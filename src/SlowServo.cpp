/// \file SlowServo.cpp
/// \author wschaeffer
/// \date 2020-10-20
/// \brief Implementation of SlowServo class

#include "SlowServo.h"

bool SlowServo::moveTo(int destination)
{
  int position = SoftRcPulseOut::read();

  if(position < destination)
  {
    position++;
  }
  else if(destination < position)
  {
    position--;
  }

  SoftRcPulseOut::write(position);
  return position == destination;
}