#ifndef AIR_CYLINDER_CONTROL_H
#define AIR_CYLINDER_CONTROL_H

#include <Arduino.h>

enum AirCylinderStates
{
  waitOnDetection = 0,
  confirmDetection = 1,
  insertValveOn = 2,
  insertValveOff = 3,
  valveOn = 4,
  valveOff = 5,
  tubeRemoved = 6,
  inactive = 7
};

class AirCylinderCtrlFSM
{
private:
  uint8_t _inPinSensor;
  uint8_t _outPinValve;
  uint8_t _outPinValveInsert;
  unsigned long _lastChangeTime;
  unsigned int _confirmTimeInterval;
  unsigned int _insertValveOnInterval;
  unsigned int _insertValveOffInterval;
  unsigned int _valveOnInterval;
  unsigned int _valveOffInterval;
  AirCylinderStates _currentState;

  unsigned long getTimeInterval(unsigned long currentTime);

public:
  AirCylinderCtrlFSM(uint8_t inPinSensor,
                     uint8_t outPinValve,
                     uint8_t outPinValve_Insert,
                     unsigned int confirmTimeInterval,
                     unsigned int insertValveOnInterval,
                     unsigned int insertValveOffInterval,
                     unsigned int valveOnInterval,
                     unsigned int valveOffInterval);
  void start(unsigned long currentTime);
  void stop(unsigned long currentTime);
  void run(unsigned long currentTime);
};

#endif