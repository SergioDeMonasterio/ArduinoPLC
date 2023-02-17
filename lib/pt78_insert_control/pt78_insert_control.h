#ifndef PT78_INSERT_CONTROL_H
#define PT78_INSERT_CONTROL_H

#include <Arduino.h>

enum pt78InsertStates
{
  initStatePT78Insert = 0,
  waitOnDetectionPT78Insert = 1,
  confirmDetectionPT78Insert = 2,
  holdTubeValveOnPT78Insert = 3,
  plugTipFeederValveOnPT78Insert =4,
  tubeInsertValveOnPT78Insert = 5,
  holdTubeValveOffPT78Insert = 6,
  tubeInsertValveOffPT78Insert = 7,
  confirmRemovalPT78Insert = 8,
  tubeRemovedPT78Insert = 9
};

extern char *pt78InsertStatesStr[];

class PT78_InsertCtrlFSM
{
private:
  uint8_t _inPinCrimperSensor;
  uint8_t _sensorDetectsOn;
  uint8_t _outPinTubeHoldValve;
  uint8_t _outPinInsertValve;
  uint8_t _outPinPTFeederValve;

  boolean _holdTubeSwitch;

  unsigned long _lastChangeTime;
  unsigned int _confirmTimeInterval;
  unsigned int _holdTubeValveOnDelay;
  unsigned int _insertValveOnDelay;
  
  unsigned int _holdTubeValveOffDelay;
  unsigned int _insertValveOffDelay;

  pt78InsertStates _currentState;
  
  boolean tubeIsDetected();
  boolean tubeIsRemoved();
  unsigned long getTimeInterval();
  boolean timeElapsed(unsigned int time);
  void changeState(pt78InsertStates nextState);


public:
  PT78_InsertCtrlFSM(    uint8_t inPinCrimperSensor,
                     uint8_t sensorDetectsOn,
                     uint8_t outPinHoldValve,
                     uint8_t outPinInsertValve,
                     uint8_t outPinPTFeederValve,

                     boolean holdTubeValveSwitch,

                     unsigned int confirmTimeInterval,
                     unsigned int holdTubeValveOnDelay,
                     unsigned int insertValveOnDelay,
                     unsigned int holdTubeValveOffDelay,
                     unsigned int insertValveOffDelay);
  void start();
  void stop();
  void run();
};

#endif