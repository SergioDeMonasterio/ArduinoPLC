#include "tube_feeder_control.h"
#include "../board_config/time_constants.h"
#include <Arduino.h>

char *tubeFeederStatesStr[] = {"feederWaitOnDetection",
                               "confirmTubeDetection",
                               "getTube",
                               "liftTube",
                               "moveTube",
                               "releaseTube",
                               "moveToInitPos",
                               "waitOnTube",
                               "feederInactive"};

TubeFeederCtrlFSM::TubeFeederCtrlFSM(
    uint8_t inPinTubeFeederSensor,
    uint8_t outPinTubeFeederValve,
    uint8_t outPinLiftingValve,
    uint8_t outPinHorMovingValve)
{
  _inPinTubeFeederSensor = inPinTubeFeederSensor;
  _outPinTubeFeederValve = outPinTubeFeederValve;
  _outPinLiftingValve = outPinLiftingValve;
  _outPinHorMovingValve = outPinHorMovingValve;

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
  changeState(moveToInitPos);
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
      if (getTimeInterval() > CONFIRM_TUBE_DETECTION_TIME_MS)
      {
        digitalWrite(_outPinTubeFeederValve, HIGH);
        changeState(getTube);
      }
    }
    else
      changeState(feederWaitOnDetection);
    break;
  case getTube:
    if (getTimeInterval() > GETTING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinLiftingValve, HIGH);
      changeState(liftTube);
    }
    break;
  case liftTube:
    if (getTimeInterval() > LIFTING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinHorMovingValve, HIGH);
      changeState(moveTube);
    }
    break;
  case moveTube:
    if (getTimeInterval() > MOVING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinLiftingValve, LOW);
      digitalWrite(_outPinTubeFeederValve, LOW);
      changeState(releaseTube);
    }
    break;
  case releaseTube:
    if (getTimeInterval() > RELEASING_TUBE_TIME_MS)
    {
      digitalWrite(_outPinHorMovingValve, LOW);
      changeState(moveToInitPos);
    }
    break;
  case moveToInitPos:
    digitalWrite(_outPinTubeFeederValve, LOW);
    digitalWrite(_outPinTubeFeederValve, LOW);
    digitalWrite(_outPinHorMovingValve, LOW);
    if (getTimeInterval() > MOVING_TUBE_TIME_MS)
      changeState(waitOnTube);
    break;
  case waitOnTube:
    changeState(feederWaitOnDetection);
    break;
  case feederInactive:
    changeState(feederInactive);
    break;
  default:
    changeState(feederWaitOnDetection);
  }
}