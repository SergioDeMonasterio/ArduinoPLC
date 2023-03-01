#include "pt78_insert_control.h"
#include <Arduino.h>
#include "../basic_functions/basic_functions.h"

char *pt78InsertStatesStr[] = {"initState",
                            "waitOnDetection",
                            "confirmDetectionPT78Insert",
                            "holdTubeValveOnPT78Insert",
                            "plugTipFeederValveOnPT78Insert",
                            "tubeInsertValveOnPT78Insert",
                            "holdTubeValveOffPT78Insert",
                            "tubeInsertValveOff",
                            "confirmRemovalPT78Insert",
                            "tubeRemovedPT78Insert"};


PT78_InsertCtrlFSM::PT78_InsertCtrlFSM(
    uint8_t inPinCrimperSensor,
    uint8_t sensorDetectsOn,
    uint8_t outPinHoldValve,
    uint8_t outPinInsertValve,
    uint8_t outPinPTFeederValve,
    
    boolean holdTubeValveSwitch,

    unsigned int confirmTimeInterval,
    unsigned int holdTubeValveOnDelay,
    unsigned int insertValveOnDelay,
    unsigned int holdTubeValveOffDelay,
    unsigned int insertValveOffDelay)
{
  _inPinCrimperSensor = inPinCrimperSensor;
  _sensorDetectsOn    = sensorDetectsOn;
  _outPinTubeHoldValve = outPinHoldValve;
  _outPinInsertValve = outPinInsertValve;
  _outPinPTFeederValve = outPinPTFeederValve;

  _holdTubeSwitch = holdTubeValveSwitch;

  _confirmTimeInterval = confirmTimeInterval;
  _insertValveOnDelay = insertValveOnDelay;
  _holdTubeValveOnDelay = holdTubeValveOnDelay;
  _holdTubeValveOffDelay = holdTubeValveOffDelay;
  _insertValveOffDelay = insertValveOffDelay;

  _currentState = initStatePT78Insert;
}

boolean PT78_InsertCtrlFSM::tubeIsDetected(){
  return objDetected(_inPinCrimperSensor,_sensorDetectsOn);
}

boolean PT78_InsertCtrlFSM::tubeIsRemoved(){
  return not objDetected(_inPinCrimperSensor,_sensorDetectsOn);
}

unsigned long PT78_InsertCtrlFSM::getTimeInterval()
{
  unsigned long timeInterval = millis() - _lastChangeTime;
  return timeInterval;
}

boolean PT78_InsertCtrlFSM::timeElapsed(unsigned int time){
  return getTimeInterval() > time;
}

void PT78_InsertCtrlFSM::changeState(pt78InsertStates nextState)
{
  _currentState = nextState;
  _lastChangeTime = millis();
  Serial.print("State: ");
  Serial.println(pt78InsertStatesStr[_currentState]);
}

void PT78_InsertCtrlFSM::start()
{
  Serial.println("Air cylinder state machine STARTED!");
  changeState(waitOnDetectionPT78Insert);
}

void PT78_InsertCtrlFSM::stop()
{
  Serial.println("Air cylinder state machine STOPPED!");
  changeState(initStatePT78Insert);
}

void PT78_InsertCtrlFSM::run()
{
  switch (_currentState)
  {
  case initStatePT78Insert:
    digitalWrite(_outPinPTFeederValve, LOW);
    digitalWrite(_outPinTubeHoldValve, LOW);
    digitalWrite(_outPinInsertValve, LOW);
    break;
  case waitOnDetectionPT78Insert:
    if (tubeIsDetected())
      changeState(confirmDetectionPT78Insert);
    break;
  case confirmDetectionPT78Insert:
    if (tubeIsDetected())
    {
      if (timeElapsed(_confirmTimeInterval)) changeState(plugTipFeederValveOnPT78Insert);
    }
    else
      changeState(waitOnDetectionPT78Insert);
    break;
  case holdTubeValveOnPT78Insert:
    digitalWrite(_outPinTubeHoldValve, HIGH);
    if (timeElapsed(_holdTubeValveOnDelay)) changeState(plugTipFeederValveOnPT78Insert);
    break;
  case plugTipFeederValveOnPT78Insert:
    digitalWrite(_outPinPTFeederValve, HIGH);
    if (timeElapsed(_insertValveOnDelay)) changeState(tubeInsertValveOnPT78Insert);
    break;
  case tubeInsertValveOnPT78Insert:
       digitalWrite(_outPinInsertValve, HIGH);
       if (timeElapsed(_holdTubeValveOnDelay)) changeState(tubeInsertValveOffPT78Insert);
    break;
  case holdTubeValveOffPT78Insert:
       if (timeElapsed(_holdTubeValveOffDelay))
       {
           digitalWrite(_outPinTubeHoldValve, LOW);
           changeState(tubeInsertValveOffPT78Insert);
       }
       break;
  case tubeInsertValveOffPT78Insert:
    digitalWrite(_outPinInsertValve, LOW);
    digitalWrite(_outPinPTFeederValve, LOW);
    if (tubeIsRemoved()){
      changeState(confirmRemovalPT78Insert);
    }
    break;
  case confirmRemovalPT78Insert:
    if (tubeIsRemoved()) {
       if (timeElapsed(_confirmTimeInterval)){
        changeState(tubeRemovedPT78Insert);
      } 
    }  
    else
      changeState(confirmRemovalPT78Insert);
    break;  
  case tubeRemovedPT78Insert:
      changeState(waitOnDetectionPT78Insert);
      break;
  default:
    changeState(waitOnDetectionPT78Insert);
  }
}
