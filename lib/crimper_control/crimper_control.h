#ifndef CRIMPER_CONTROL_H
#define CRIMPER_CONTROL_H

#include <Arduino.h>

enum CrimperStates
{
  waitOnDetection = 0,
  confirmDetection = 1,
  insertValveOn = 2,
  crimpValveOn = 3,
  crimpValveOff = 4,
  insertValveOff = 5,
  confirmRemoval = 6,
  tubeRemoved = 7,
  inactive = 8
};

extern char *crimperStatesStr[];

class CrimperCtrlFSM
{
private:
  uint8_t _inPinCrimperSensor;
  uint8_t _outPinCrimpValve;
  uint8_t _outPinInsertValve;
  unsigned long _lastChangeTime;
  unsigned int _confirmTimeInterval;
  unsigned int _insertValveOnInterval;
  unsigned int _insertValveOffInterval;
  unsigned int _valveOnInterval;
  unsigned int _valveOffInterval;
  CrimperStates _currentState;

  unsigned long getTimeInterval();
  void changeState(CrimperStates nextState);
  
public:
  CrimperCtrlFSM(uint8_t inPinSensor,
                     uint8_t outPinValve,
                     uint8_t outPinValve_Insert,
                     unsigned int confirmTimeInterval,
                     unsigned int insertValveOnInterval,
                     unsigned int insertValveOffInterval,
                     unsigned int valveOnInterval,
                     unsigned int valveOffInterval);
  void start();
  void stop();
  void run();
};

#endif