/// \file main.cpp
/// \author wschaeffer
/// \date 2020-10-30
/// \brief Most joyful yet useless machine ever!

#include <Arduino.h>

#include "SlowServo.h"

const int DELAY     = 8;      // milliseconds
const long shutdown = 20000;  // milliseconds

const int lidOpen  = 10;
const int lidClose = 90;
const int lidOff   = 110;

const int armPeek   = 100;
const int armClose  = 175;
const int armSwitch = 75;

const int ledPin      = 3;
const int switchPin   = 2;
const int lidServoPin = 9;
const int armServoPin = 10;

SlowServo armServo;
SlowServo lidServo;

const long shutdownTicks = shutdown / DELAY;
long shutdownTimer       = 0;

bool previousSwitch = true;
bool switched       = false;

enum class State
{
  IDLE,
  OPENING,
  CLOSING,
  PEEKING,
  SWITCHING
} currentState;

enum class Event
{
  SWITCH,
  TICK
};

void setup()
{
  armServo.attach(armServoPin);
  lidServo.attach(lidServoPin);
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  armServo.write(armClose);
  lidServo.write(lidClose);
}

State decision(State a, State b, int weight = 5)
{
  randomSeed(analogRead(0));
  long rand = random(10);
  return rand < weight ? a : b;
}

State handleIdleState(Event event)
{
  State result = State::IDLE;
  switch(event)
  {
    case Event::SWITCH:
      switched      = false;
      shutdownTimer = 0;
      result        = State::OPENING;
      break;
    case Event::TICK:
      shutdownTimer++;
      if(shutdownTimer >= shutdownTicks)
      {
        lidServo.write(lidOff);
      }
      break;
  }
  return result;
}

State handleOpeningState(Event event)
{
  State result = State::OPENING;
  switch(event)
  {
    case Event::TICK:
      if(lidServo.moveTo(lidOpen) && armServo.moveTo(armClose))
      {
        int weight      = !switched ? 7 : 2;
        State nextState = decision(State::PEEKING, State::CLOSING, weight);
        if(nextState != State::CLOSING)
        {
          digitalWrite(ledPin, HIGH);
        }
        else
        {
          digitalWrite(ledPin, LOW);
        }
        result = nextState;
      }
      break;
    default:
      // empty
      break;
  }
  return result;
}

State handleClosingState(Event event)
{
  State result = State::CLOSING;
  switch(event)
  {
    case Event::SWITCH:
      result = State::OPENING;
      break;
    case Event::TICK:
      if(lidServo.moveTo(lidClose))
      {
        if(switched)
        {
          result = decision(State::IDLE, State::OPENING, 8);
        }
        else
        {
          result = State::OPENING;
        }
      }
      break;
  }
  return result;
}

State handlePeekingState(Event event)
{
  State result = State::PEEKING;
  switch(event)
  {
    case Event::TICK:
      if(armServo.moveTo(armPeek))
      {
        if(!switched)
        {
          result = decision(State::SWITCHING, State::OPENING, 7);
        }
        else
        {
          result = State::OPENING;
        }
      }
      break;
    default:
      // empty
      break;
  }
  return result;
}

State handleSwitchingState(Event event)
{
  State result = State::SWITCHING;
  switch(event)
  {
    case Event::TICK:
      if(armServo.moveTo(armSwitch))
      {
        switched = true;
        result   = State::OPENING;
      }
      break;
    default:
      // empty
      break;
  }
  return result;
}

void HandleEvent(Event event)
{
  switch(currentState)
  {
    case State::IDLE:
      currentState = handleIdleState(event);
      break;
    case State::OPENING:
      currentState = handleOpeningState(event);
      break;
    case State::CLOSING:
      currentState = handleClosingState(event);
      break;
    case State::PEEKING:
      currentState = handlePeekingState(event);
      break;
    case State::SWITCHING:
      currentState = handleSwitchingState(event);
      break;
  }
}

void loop()
{
  Event event        = Event::TICK;
  bool currentSwitch = digitalRead(switchPin);
  if(currentSwitch != previousSwitch)
  {
    previousSwitch = currentSwitch;
    if(!currentSwitch)
    {
      event = Event::SWITCH;
    }
  }

  HandleEvent(event);
  delay(DELAY);
}