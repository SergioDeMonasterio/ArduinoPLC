#include "crimper_control.h"
#include <Arduino.h>
#include "../basic_functions/basic_functions.h"


char *crimperStatesStr[] = {"initState",
                            "waitOnDetection",
                            "confirmDetection",
                            "plugTipFeederValveOn",
                            "tubeInsertValveOn",
                            "crimpValveOn",
                            "crimpValveOff",
                            "insertValvesOff",
                            "confirmRemoval",
                            "tubeRemoved"};

CrimperCtrlFSM::CrimperCtrlFSM(
    uint8_t inPinCrimperSensor,
    uint8_t sensorDetectsOn,
    uint8_t outPinCrimperValve,
    uint8_t outPinInsertValve,
    uint8_t outPinPTFeederValve,
    
    boolean crimpingSwitch,

    unsigned int confirmTimeInterval,
    unsigned int insertValveOnDelay,
    unsigned int crimperValveOnDelay,
    unsigned int crimperValveOffDelay,
    unsigned int insertValveOffDelay)
{
  _inPinCrimperSensor = inPinCrimperSensor;
  _sensorDetectsOn    = sensorDetectsOn;
  _outPinCrimperValve = outPinCrimperValve;
  _outPinInsertValve = outPinInsertValve;
  _outPinPTFeederValve = outPinPTFeederValve;

  _crimpingSwitch = crimpingSwitch;

  _confirmTimeInterval = confirmTimeInterval;
  _insertValveOnDelay = insertValveOnDelay;
  _crimperValveOnDelay = crimperValveOnDelay;
  _crimperValveOffDelay = crimperValveOffDelay;
  _insertValveOffDelay = insertValveOffDelay;

  _currentState = initState;
}

boolean CrimperCtrlFSM::tubeIsDetected(){
  return objDetected(_inPinCrimperSensor,_sensorDetectsOn);
}

boolean CrimperCtrlFSM::tubeIsRemoved(){
  return not objDetected(_inPinCrimperSensor,_sensorDetectsOn);
}

unsigned long CrimperCtrlFSM::getTimeInterval()
{
  unsigned long timeInterval = millis() - _lastChangeTime;
  return timeInterval;
}

boolean CrimperCtrlFSM::timeElapsed(unsigned int time){
  return getTimeInterval() > time;
}

void CrimperCtrlFSM::changeState(CrimperStates nextState)
{
  _currentState = nextState;
  _lastChangeTime = millis();
  Serial.print("State: ");
  Serial.println(crimperStatesStr[_currentState]);
}

void CrimperCtrlFSM::start()
{
  Serial.println("Air cylinder state machine STARTED!");
  changeState(waitOnDetection);
}

void CrimperCtrlFSM::stop()
{
  Serial.println("Air cylinder state machine STOPPED!");
  changeState(initState);
}

void CrimperCtrlFSM::run()
{
  switch (_currentState)
  {
  case initState:
    digitalWrite(_outPinPTFeederValve, LOW);
    digitalWrite(_outPinCrimperValve, LOW);
    digitalWrite(_outPinInsertValve, LOW);
    break;
  case waitOnDetection:
    if (tubeIsDetected())
      changeState(confirmDetection);
    break;
  case confirmDetection:
    if (tubeIsDetected())
    {
      if (timeElapsed(_confirmTimeInterval)) 
      {
        digitalWrite(_outPinPTFeederValve, HIGH);
        changeState(plugTipFeederValveOn);
      }
    }
    else
      changeState(waitOnDetection);
    break;
  case plugTipFeederValveOn:
    if (timeElapsed(_insertValveOnDelay))
    {
      digitalWrite(_outPinInsertValve, HIGH);
      changeState(tubeInsertValveOn);
    }
    break;
  case tubeInsertValveOn:
       if (timeElapsed(_crimperValveOnDelay))
    {
      
      if (_crimpingSwitch) {
        digitalWrite(_outPinCrimperValve, HIGH);
        changeState(crimpValveOn);
      }
      else {
           digitalWrite(_outPinInsertValve, LOW);
           digitalWrite(_outPinPTFeederValve, LOW);
           changeState(insertValvesOff);
      }
    }
    break;
  case crimpValveOn:
    if (timeElapsed(_crimperValveOffDelay))
    {
      digitalWrite(_outPinCrimperValve, LOW);
      changeState(crimpValveOff);
    }
    break;
  case crimpValveOff:
    if (timeElapsed(_insertValveOffDelay)) 
    {
      digitalWrite(_outPinInsertValve, LOW);
      digitalWrite(_outPinPTFeederValve, LOW);
      changeState(insertValvesOff);
    }
    break;
  case insertValvesOff:
    if (tubeIsRemoved()){
      changeState(confirmRemoval);
    }
    break;
  case confirmRemoval:
    if (tubeIsRemoved()) {
       if (timeElapsed(_confirmTimeInterval)){
        changeState(tubeRemoved);
      } 
    }  
    else
      changeState(confirmRemoval);
    break;  
  case tubeRemoved:
      changeState(waitOnDetection);
      break;
  default:
    changeState(waitOnDetection);
  }
}
