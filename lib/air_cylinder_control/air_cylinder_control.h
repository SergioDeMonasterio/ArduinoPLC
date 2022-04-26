#ifndef AIR_CYLINDER_CONTROL_H
#define AIR_CYLINDER_CONTROL_H

#include <Arduino.h>

enum AirCylinderStates
{
  waitOnDetection = 0,
  confirmDetection = 1,
  valveOn = 2,
  valveOff = 3,
  tubeRemoved = 4,
  inactive = 5
};

class AirCylinderCtrlFSM
{
private:
  uint8_t _inPinSensor;
  uint8_t _outPinValve;
  unsigned long _lastChangeTime;
  unsigned int _confirmTimeInterval;
  unsigned int _valveOnInterval;
  unsigned int _valveOffInterval;
  AirCylinderStates _currentState;

  unsigned long getTimeInterval(unsigned long currentTime);

public:
  AirCylinderCtrlFSM(uint8_t inPinSensor,
                     uint8_t outPinValve,
                     unsigned int confirmTimeInterval,
                     unsigned int valveOnInterval,
                     unsigned int valveOffInterval);
  void start(unsigned long currentTime);
  void stop(unsigned long currentTime);
  void run(unsigned long currentTime);
};

#endif