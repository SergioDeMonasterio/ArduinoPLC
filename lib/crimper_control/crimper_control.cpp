#include "crimper_control.h"
#include <Arduino.h>

char *crimperStatesStr[] = {"waitOnDetection",
                            "confirmDetection",
                            "insertValveOn",
                            "crimpValveOn",
                            "crimpValveOff",
                            "insertValveOff",
                            "confirmRemoval",
                            "tubeRemoved",
                            "inactive"};

CrimperCtrlFSM::CrimperCtrlFSM(
    uint8_t inPinCrimperSensor,
    uint8_t outPinValve,
    uint8_t outPinValveInsert,
    unsigned int confirmTimeInterval,
    unsigned int insertValveOnInterval,
    unsigned int insertValveOffInterval,
    unsigned int valveOnInterval,
    unsigned int valveOffInterval)
{
  _inPinCrimperSensor = inPinCrimperSensor;
  _outPinCrimpValve = outPinValve;
  _outPinInsertValve = outPinValveInsert;

  _confirmTimeInterval = confirmTimeInterval;
  _valveOnInterval = valveOnInterval;
  _valveOffInterval = valveOffInterval;
  _insertValveOnInterval = insertValveOnInterval;
  _insertValveOffInterval = insertValveOffInterval;

  _currentState = inactive;

  Serial.print("Input Pin: ");
  Serial.println(inPinCrimperSensor);
  Serial.print("Output Pin: ");
  Serial.println(outPinValve);
}

unsigned long CrimperCtrlFSM::getTimeInterval()
{
  unsigned long timeInterval = millis() - _lastChangeTime;
  return timeInterval;
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
  changeState(inactive);
}

void CrimperCtrlFSM::run()
{

  switch (_currentState)
  {
  case waitOnDetection:
    if (digitalRead(_inPinCrimperSensor) == LOW)
      changeState(confirmDetection);
    break;
  case confirmDetection:
    if (digitalRead(_inPinCrimperSensor) == LOW)
    {
      if (_confirmTimeInterval < getTimeInterval())
      {
        Serial.print("Current time interval: ");
        Serial.println(getTimeInterval());
        digitalWrite(_outPinInsertValve, HIGH);
        changeState(insertValveOn);
      }
    }
    else
      changeState(waitOnDetection);
    break;
  case insertValveOn:
    if (_insertValveOnInterval < getTimeInterval())
    {
      digitalWrite(_outPinCrimpValve, HIGH);
      changeState(crimpValveOn);
    }
    break;
  case crimpValveOn:
    if (_valveOnInterval < getTimeInterval())
    {
      digitalWrite(_outPinCrimpValve, LOW);
      changeState(crimpValveOff);
    }
    break;
  case crimpValveOff:
    if (_insertValveOffInterval < getTimeInterval()) 
    {
      digitalWrite(_outPinInsertValve, LOW);
      changeState(insertValveOff);
    }
    break;
  case insertValveOff:
    if (digitalRead(_inPinCrimperSensor) == HIGH)
      changeState(confirmRemoval);
    break;
  case confirmRemoval:
    if (digitalRead(_inPinCrimperSensor) == HIGH)
    {
      if (_confirmTimeInterval < getTimeInterval())
        changeState(tubeRemoved);
    }
    else
      changeState(insertValveOff);
    break;  
  case tubeRemoved:
    if (_valveOffInterval < getTimeInterval())
      changeState(waitOnDetection);
    break;
  case inactive:
    changeState(inactive);
    break;
  default:
    changeState(waitOnDetection);
  }
}
