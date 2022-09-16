#include "air_cylinder_control.h"
#include <Arduino.h>

char *crimperStatesStr[] = {"waitOnDetection", "confirmDetection", "insertValveOn",
                     "insertValveOff", "valveOn","valveOff","tubeRemoved","inactive" };

AirCylinderCtrlFSM::AirCylinderCtrlFSM(
    uint8_t inPinSensor,
    uint8_t outPinValve,
    uint8_t outPinValveInsert,
    unsigned int confirmTimeInterval,
    unsigned int insertValveOnInterval,
    unsigned int insertValveOffInterval,
    unsigned int valveOnInterval,
    unsigned int valveOffInterval)
{
  _inPinSensor = inPinSensor;
  _outPinValve = outPinValve;
  _outPinValveInsert = outPinValveInsert;

  _confirmTimeInterval = confirmTimeInterval;
  _valveOnInterval = valveOnInterval;
  _valveOffInterval = valveOffInterval;
  _insertValveOnInterval = insertValveOnInterval;
  _insertValveOffInterval = insertValveOffInterval;

  _currentState = inactive;

  Serial.print("Input Pin: ");
  Serial.println(inPinSensor);
  Serial.print("Output Pin: ");
  Serial.println(outPinValve);
}

unsigned long AirCylinderCtrlFSM::getTimeInterval(unsigned long currentTime)
{
  unsigned long timeInterval = currentTime - _lastChangeTime;
  return timeInterval;
}

void AirCylinderCtrlFSM::changeState(CrimperStates nextState, unsigned long currentTime){
  _currentState = nextState;
  _lastChangeTime = currentTime;
  Serial.print("State: "); Serial.println(crimperStatesStr[_currentState]);
}

void AirCylinderCtrlFSM::start(unsigned long currentTime)
{
  Serial.println("Air cylinder state machine STARTED!");
  changeState(waitOnDetection, currentTime);
}

void AirCylinderCtrlFSM::stop(unsigned long currentTime)
{
  Serial.println("Air cylinder state machine STOPPED!");
  changeState(inactive, currentTime);
}

void AirCylinderCtrlFSM::run(unsigned long currentTime)
{

  switch (_currentState)
  {
  case waitOnDetection:
    if (digitalRead(_inPinSensor) == LOW) changeState(confirmDetection, currentTime);
    break;
  case confirmDetection:
    if (digitalRead(_inPinSensor) == LOW)
    {
      if (_confirmTimeInterval < getTimeInterval(currentTime))
      {
        Serial.print("Current time interval: "); Serial.println(getTimeInterval(currentTime));
        digitalWrite(_outPinValveInsert, HIGH);
        changeState(insertValveOn, currentTime);
      }
    }
    else changeState(waitOnDetection, currentTime);
    break;
  case insertValveOn:
    if (_insertValveOnInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValveInsert, LOW);
      changeState(insertValveOff, currentTime);
    }
    break;  
  case insertValveOff:
  if (_insertValveOffInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValve, HIGH);
      changeState(valveOn, currentTime);
    }
    break;  
  case valveOn:
    if (_valveOnInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValve, LOW);
      changeState(valveOff, currentTime);
    }
    break;
  case valveOff:
    if (digitalRead(_inPinSensor) == HIGH)
      changeState(tubeRemoved, currentTime);
    break;
  case tubeRemoved:
    if (_valveOffInterval < getTimeInterval(currentTime))
      changeState(waitOnDetection, currentTime);
    break;
  case inactive:
    changeState(inactive, currentTime);
    break; // optional
  default: // Optional
    changeState(waitOnDetection, currentTime);
  }
}
