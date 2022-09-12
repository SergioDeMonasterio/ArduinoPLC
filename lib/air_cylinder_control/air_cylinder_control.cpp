#include "air_cylinder_control.h"
#include <Arduino.h>

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

void AirCylinderCtrlFSM::start(unsigned long currentTime)
{
  _currentState = waitOnDetection;
  _lastChangeTime = currentTime;
  Serial.println("Air cylinder state machine STARTED!");
  Serial.println("State: waitOnDetection");
}

void AirCylinderCtrlFSM::stop(unsigned long currentTime)
{
  _currentState = inactive;
  _lastChangeTime = currentTime;
  Serial.println("Air cylinder state machine STOPPED!");
}

void AirCylinderCtrlFSM::run(unsigned long currentTime)
{

  switch (_currentState)
  {
  case waitOnDetection:
    if (digitalRead(_inPinSensor) == LOW)
    {
      _lastChangeTime = currentTime;
      _currentState = confirmDetection;
      Serial.println("State: confirmDetection");
    }
    break;
  case confirmDetection:
    if (digitalRead(_inPinSensor) == LOW)
    {
      if (_confirmTimeInterval < getTimeInterval(currentTime))
      {
        Serial.print("Confirm time interval: ");
        Serial.println(_confirmTimeInterval);
        Serial.print("Current time interval: ");
        Serial.println(getTimeInterval(currentTime));
        digitalWrite(_outPinValveInsert, HIGH);
        _lastChangeTime = currentTime;
        _currentState = insertValveOn;
        Serial.println("State: insertValveOn");
      }
    }
    else
    {
      _currentState = waitOnDetection;
      _lastChangeTime = currentTime;
      Serial.println("State: waitOnDetection");
    }
    break;
  case insertValveOn:
    if (_insertValveOnInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValveInsert, LOW);
      _currentState = insertValveOff;
      _lastChangeTime = currentTime;
      Serial.println("State: insertValveOff");
    }
    break;  
  case insertValveOff:
  if (_insertValveOffInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValve, HIGH);
     _currentState = valveOn;
      _lastChangeTime = currentTime;
      Serial.println("State: valveOn");
    }
    break;  
  case valveOn:
    if (_valveOnInterval < getTimeInterval(currentTime))
    {
      digitalWrite(_outPinValve, LOW);
      _currentState = valveOff;
      _lastChangeTime = currentTime;
      Serial.println("State: valveOff");
    }
    break;
  case valveOff:
    if (digitalRead(_inPinSensor) == HIGH)
    {
      _currentState = tubeRemoved;
      _lastChangeTime = currentTime;
      Serial.println("State: tubeRemoved");
    }
    break;
  case tubeRemoved:
    if (_valveOffInterval < getTimeInterval(currentTime))
    {
      _currentState = waitOnDetection;
      _lastChangeTime = currentTime;
      Serial.println("State: waitOnDetection");
    }
    break;
  case inactive:
    _lastChangeTime = currentTime;
    Serial.println("State: inactive");
    break; // optional

  // you can have any number of case statements.
  default: // Optional
    _currentState = waitOnDetection;
    Serial.println("State: waitOnDetection");
  }
}
