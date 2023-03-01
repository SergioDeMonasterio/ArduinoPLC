#include <Arduino.h>
#include "tube_feeder_control.h"
#include "../board_config/system_constants.h"
#include "../basic_functions/basic_functions.h"

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
    uint8_t sensorDetectsOn,
    uint8_t inPinBreaker,
    uint8_t outPinTubeFeederArm,
    uint8_t outPinLifter,
    uint8_t outPinHorMover)
{
  _inPinTubeFeederSensor = inPinTubeFeederSensor;
  _sensorDetectsOn    = sensorDetectsOn;
  _inPinBreaker = inPinBreaker;
  _outPinTubeFeederArm = outPinTubeFeederArm;
  _outPinLifter = outPinLifter;
  _outPinHorMover = outPinHorMover;

  _currentState = feederInactive;
}

boolean TubeFeederCtrlFSM::tubeIsDetected(){
  return objDetected(_inPinTubeFeederSensor,_sensorDetectsOn);
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
    if (tubeIsDetected())
      changeState(confirmTubeDetection);
    break;
  case confirmTubeDetection:
    if (tubeIsDetected())
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
    digitalWrite(_outPinTubeFeederArm, FEEDER_ARM_MOVE_IN);
    digitalWrite(_outPinHorMover, FEEDER_HORIZONTAL_INIT_POS);
    digitalWrite(_outPinLifter, FEEDER_DOWN_TUBE);
    if (getTimeInterval() > FEEDER_HOR_MOVING_TUBE_TIME_MS) changeState(waitOnTubeFromFeeder);
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

  if ( digitalRead(_inPinBreaker) == HIGH ){
    changeState(feederWaitOnDetection);
    Serial.println("Breaker activated !!!!!");
    }
}