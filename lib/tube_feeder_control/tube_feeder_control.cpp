#include <Arduino.h>
#include "tube_feeder_control.h"
#include "../board_config/system_constants.h"

char *tubeFeederStatesStr[] = {"feederWaitOnDetection",
                               "confirmTubeDetection",
                               "getTubeFromFeeder",
                               "liftTube",
                               "moveTubeToOuterPos",
                               "downTube",
                               "moveToInitPos",
                               "waitOnTubeFromFeeder",
                               "feederInactive"};

TubeFeederCtrlFSM::TubeFeederCtrlFSM(
    uint8_t inPinTubeFeederSensor,
    uint8_t outPinTubeFeederValve,
    uint8_t outPinLiftingValve,
    uint8_t outPinHorMovingValve)
{
  _inPinTubeFeederSensor = inPinTubeFeederSensor;
  _outPinTubeFeederArm = outPinTubeFeederValve;
  _outPinLifter = outPinLiftingValve;
  _outPinHorMover = outPinHorMovingValve;

  _currentState = feederInactive;
}

unsigned long TubeFeederCtrlFSM::getTimeInterval()
{
  unsigned long timeInterval = millis() - _lastChangeTime;
  return timeInterval;
}

void TubeFeederCtrlFSM::changeState(TubeFeederStates nextState)
{
  _currentState = nextState;
  _lastChangeTime = millis();
  Serial.print("Tube Feeder State: ");
  Serial.println(tubeFeederStatesStr[_currentState]);
}

void TubeFeederCtrlFSM::start()
{
  Serial.println("Tube feeder STARTED!");
  changeState(moveTubeToInitPos);
}

void TubeFeederCtrlFSM::stop()
{
  Serial.println("Tube feeder STOPPED!");
  changeState(feederInactive);
}

void TubeFeederCtrlFSM::run()
{

  switch (_currentState)
  {
  case feederWaitOnDetection:
    if (digitalRead(_inPinTubeFeederSensor) == LOW)
      changeState(confirmTubeDetection);
    break;
  case confirmTubeDetection:
    if (digitalRead(_inPinTubeFeederSensor) == LOW)
    {
      if (getTimeInterval() > FEEDER_CONFIRM_TUBE_DETECTION_TIME_MS)
      {
        digitalWrite(_outPinTubeFeederArm, FEEDER_ARM_MOVE_OUT);
        changeState(getTubeFromFeeder);
      }
    }
    else
      changeState(feederWaitOnDetection);
    break;
  case getTubeFromFeeder:
    if (getTimeInterval() > FEEDER_ARM_GETTING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinLifter, FEEDER_LIFT_TUBE);
      changeState(liftTube);
    }
    break;
  case liftTube:
    if (getTimeInterval() > FEEDER_LIFTING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinHorMover, FEEDER_HORIZONTAL_OUT_POS);
      changeState(moveTubeToOuterPos);
    }
    break;
  case moveTubeToOuterPos:
    if (getTimeInterval() > FEEDER_HOR_MOVING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinLifter, FEEDER_DOWN_TUBE);
      digitalWrite(_outPinTubeFeederArm, FEEDER_ARM_MOVE_IN);
      changeState(downTube);
    }
    break;
  case downTube:
    if (getTimeInterval() > FEEDER_DOWN_TUBE_TIME_MS)
    {
      digitalWrite(_outPinHorMover, FEEDER_HORIZONTAL_INIT_POS);
      changeState(moveTubeToInitPos);
    }
    break;
  case moveTubeToInitPos:
    if (getTimeInterval() > FEEDER_HOR_MOVING_TUBE_TIME_MS)
      digitalWrite(_outPinTubeFeederArm, FEEDER_ARM_MOVE_OUT);
    changeState(waitOnTubeFromFeeder);
    break;
  case waitOnTubeFromFeeder:
    changeState(feederWaitOnDetection);
    break;
  case feederInactive:
    changeState(feederInactive);
    break;
  default:
    changeState(feederWaitOnDetection);
  }
}