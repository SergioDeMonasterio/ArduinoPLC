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

unsigned long AirCylinderCtrlFSM::getTimeInterval()
{
  unsigned long timeInterval =  millis() - _lastChangeTime;
  return timeInterval;
}

void AirCylinderCtrlFSM::changeState(CrimperStates nextState){
  _currentState = nextState;
  _lastChangeTime = millis();
  Serial.print("State: "); Serial.println(crimperStatesStr[_currentState]);
}

void AirCylinderCtrlFSM::start()
{
  Serial.println("Air cylinder state machine STARTED!");
  changeState(waitOnDetection);
}

void AirCylinderCtrlFSM::stop()
{
  Serial.println("Air cylinder state machine STOPPED!");
  changeState(inactive);
}

void AirCylinderCtrlFSM::run()
{

  switch (_currentState)
  {
  case waitOnDetection:
    if (digitalRead(_inPinSensor) == LOW) changeState(confirmDetection);
    break;
  case confirmDetection:
    if (digitalRead(_inPinSensor) == LOW)
    {
      if (_confirmTimeInterval < getTimeInterval())
      {
        Serial.print("Current time interval: "); Serial.println(getTimeInterval());
        digitalWrite(_outPinValveInsert, HIGH);
        changeState(insertValveOn);
      }
    }
    else changeState(waitOnDetection);
    break;
  case insertValveOn:
    if (_insertValveOnInterval < getTimeInterval())
    {
      digitalWrite(_outPinValveInsert, LOW);
      changeState(insertValveOff);
    }
    break;  
  case insertValveOff:
  if (_insertValveOffInterval < getTimeInterval())
    {
      digitalWrite(_outPinValve, HIGH);
      changeState(valveOn);
    }
    break;  
  case valveOn:
    if (_valveOnInterval < getTimeInterval())
    {
      digitalWrite(_outPinValve, LOW);
      changeState(valveOff);
    }
    break;
  case valveOff:
    if (digitalRead(_inPinSensor) == HIGH)
      changeState(tubeRemoved);
    break;
  case tubeRemoved:
    if (_valveOffInterval < getTimeInterval())
      changeState(waitOnDetection);
    break;
  case inactive:
    changeState(inactive);
    break; // optional
  default: // Optional
    changeState(waitOnDetection);
  }
}
