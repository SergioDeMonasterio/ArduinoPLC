#ifndef TUBE_FEEDER_CONTROL_H
#define TUBE_FEEDER_CONTROL_H

#include <Arduino.h>

enum TubeFeederStates
{
  feederWaitOnDetection = 0,
  confirmTubeDetection = 1,
  getTube = 2,
  liftTube = 3,
  moveTube = 4,
  releaseTube = 5,
  moveToInitPos = 6,
  waitOnTube = 7,
  feederInactive = 8
};

extern char *tubeFeederStatesStr[];

class TubeFeederCtrlFSM
{
private:
  uint8_t _inPinTubeFeederSensor;
  uint8_t _outPinTubeFeederValve;
  uint8_t _outPinLiftingValve;
  uint8_t _outPinHorMovingValve;
  unsigned long _lastChangeTime;
  TubeFeederStates _currentState;

  unsigned long getTimeInterval();
  void changeState(TubeFeederStates nextState);

public:
  TubeFeederCtrlFSM(uint8_t inPinTubeFeederSensor,
                    uint8_t outPinTubeFeederValve,
                    uint8_t outPinLiftingValve,
                    uint8_t outPinHorMovingValve);
  void start();
  void stop();
  void run();
};

#endif