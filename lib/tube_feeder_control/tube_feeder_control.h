#ifndef TUBE_FEEDER_CONTROL_H
#define TUBE_FEEDER_CONTROL_H

#include <Arduino.h>

enum TubeFeederStates
{
  feederWaitOnDetection = 0,
  confirmTubeDetection = 1,
  getTubeFromFeeder = 2,
  liftTube = 3,
  moveTubeToOuterPos = 4,
  downTube = 5,
  moveTubeToInitPos = 6,
  waitOnTubeFromFeeder = 7,
  feederInactive = 8
};

extern char *tubeFeederStatesStr[];

class TubeFeederCtrlFSM
{
private:
  uint8_t _inPinTubeFeederSensor;
  uint8_t _sensorDetectsOn;
  uint8_t _inPinBreaker;
  uint8_t _outPinTubeFeederArm;
  uint8_t _outPinLifter;
  uint8_t _outPinHorMover;
  unsigned long _lastChangeTime;
  TubeFeederStates _currentState;

  boolean tubeIsDetected();
  unsigned long getTimeInterval();
  void changeState(TubeFeederStates nextState);

public:
  TubeFeederCtrlFSM(uint8_t inPinTubeFeederSensor,
                    uint8_t sensorDetectsOn,
                    uint8_t inPinBreaker,
                    uint8_t outPinTubeFeederArm,
                    uint8_t outPinLifter,
                    uint8_t outPinHorMover);
  void start();
  void stop();
  void run();
};

#endif