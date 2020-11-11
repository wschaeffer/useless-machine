/// \file SlowServo.h
/// \author wschaeffer
/// \date 2020-10-20
/// \brief Move servo slowly to destination based on ticks.

#pragma once

#include <SoftRcPulseOut.h>

class SlowServo : public SoftRcPulseOut
{
 public:
  SlowServo() = default;

  /// Call this function repeatedly to move servo to destination.
  /// Speed is inherent to time between each call.
  /// \param destination
  /// \return Destination reached = true
  bool moveTo(int destination);
};
