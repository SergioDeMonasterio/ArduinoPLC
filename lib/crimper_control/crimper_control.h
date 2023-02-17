#ifndef CRIMPER_CONTROL_H
#define CRIMPER_CONTROL_H

#include <Arduino.h>

enum CrimperStates
{
  initState = 0,
  waitOnDetection = 1,
  confirmDetection = 2,
  plugTipFeederValveOn =3,
  tubeInsertValveOn = 4,
  crimpValveOn = 5,
  crimpValveOff = 6,
  insertValvesOff = 7,
  confirmRemoval = 8,
  tubeRemoved = 9
};

extern char *crimperStatesStr[];

class CrimperCtrlFSM
{
private:
  uint8_t _inPinCrimperSensor;
  uint8_t _sensorDetectsOn;
  uint8_t _outPinCrimperValve;
  uint8_t _outPinInsertValve;
  uint8_t _outPinPTFeederValve;

  boolean _crimpingSwitch;

  unsigned long _lastChangeTime;
  unsigned int _confirmTimeInterval;
  unsigned int _insertValveOnDelay;
  unsigned int _crimperValveOnDelay;
  unsigned int _crimperValveOffDelay;
  unsigned int _insertValveOffDelay;

  CrimperStates _currentState;
  
  boolean tubeIsDetected();
  boolean tubeIsRemoved();
  unsigned long getTimeInterval();
  boolean timeElapsed(unsigned int time);
  void changeState(CrimperStates nextState);


public:
  CrimperCtrlFSM(    uint8_t inPinCrimperSensor,
                     uint8_t sensorDetectsOn,
                     uint8_t outPinCrimperValve,
                     uint8_t outPinInsertValve,
                     uint8_t outPinPTFeederValve,

                     boolean crimpingSwitch,

                     unsigned int confirmTimeInterval,
                     unsigned int insertValveOnDelay,
                     unsigned int crimperValveOnDelay,
                     unsigned int crimperValveOffDelay,
                     unsigned int insertValveOffDelay);
  void start();
  void stop();
  void run();
};

#endif