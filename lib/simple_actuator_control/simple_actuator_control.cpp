#include <Arduino.h>
#include "simple_actuator_control.h"
#include "../basic_functions/basic_functions.h"

char *SACStatesStr[] = {"initPosition",
                        "waitOnObjDetection",
                        "timedActuatorControl",
                        "confirmObjRemoval",
                        "inactiveState"};

SimpleActuatorControl::SimpleActuatorControl(char unitName[20],                               // Unit name for logging max. 20 chars
                                             boolean sensorDrivenOperation,                   // Sensor detection is activation the operation? TRUE / FALSE
                                             uint8_t inPinSensor,                             // Sensor input pin: Please use in pins defined in board config file
                                             uint8_t sensorDetectsOn,                         // Sensor detects on: HIGH / LOW depending on its type: PNP / NPN
                                             uint16_t confirmTimeInterval,                    // Sensor detection confirm time interval
                                             uint16_t actuationCycle,                         // Full actuation cycle time
                                             uint8_t actuatorsQty,                            // Quantity of all simple (ditial outputs) actuators
                                             actuatorPinsAndTimes actOutPinsAndSwitchTimes[]) // Output timing matrix
{
  strcpy(_unitName, unitName);
  _sensorDrivenOperation = sensorDrivenOperation;
  _inPinSensor = inPinSensor;
  _sensorDetectsOn = sensorDetectsOn;
  _confirmTimeInterval = confirmTimeInterval;
  _actuationCycle = actuationCycle;
  _actuatorsQty = actuatorsQty;
  // Copy the actuators pins and time matrix to the private variable (unchangable after initiation)
  for (short i = 0; i < actuatorsQty; ++i)
    _actOutPinsAndSwitchTimes[i] = actOutPinsAndSwitchTimes[i];

  _currentState = initPosition;
}

// Moves all the actuators in their init state: all outPins => LOW
void SimpleActuatorControl::actuatorsInitState()
{
  for (short i = 0; i < _actuatorsQty; ++i)
    digitalWrite(_actOutPinsAndSwitchTimes[i].boardPin, LOW);
}

boolean SimpleActuatorControl::objIsDetected()
{
  return objDetected(_inPinSensor, _sensorDetectsOn);
}

boolean SimpleActuatorControl::objIsRemoved()
{
  return not objDetected(_inPinSensor, _sensorDetectsOn);
}

unsigned long SimpleActuatorControl::getTimeInterval()
{
  unsigned long timeInterval = millis() - _lastChangeTime;
  return timeInterval;
}

boolean SimpleActuatorControl::timeElapsed(unsigned int time)
{
  return getTimeInterval() > time;
}
// Set the outputs according to the defined timings
void SimpleActuatorControl::setOutputs()
{
  for (unsigned int i = 0; i < _actuatorsQty; ++i)
  {
    if (timeElapsed(_actOutPinsAndSwitchTimes[i].stopActTime))
      digitalWrite(_actOutPinsAndSwitchTimes[i].boardPin, LOW);
    else if (timeElapsed(_actOutPinsAndSwitchTimes[i].startActTime))
      digitalWrite(_actOutPinsAndSwitchTimes[i].boardPin, HIGH);
  }
}

void SimpleActuatorControl::changeState(SACStates nextState)
{
  if (_currentState != nextState)
  {
    Serial.print(_unitName);
    Serial.print(" State: ");
    Serial.println(SACStatesStr[nextState]);
  }
  _currentState = nextState;
  _lastChangeTime = millis();
}

void SimpleActuatorControl::start()
{
  Serial.print(_unitName);
  Serial.println(" STARTED!");
  changeState(waitOnObjDetection);
}

void SimpleActuatorControl::stop()
{
  Serial.print(_unitName);
  Serial.println(" STOPPED!");
  changeState(initPosition);
}

unsigned int SimpleActuatorControl::run()
{
  switch (_currentState)
  {
  case initPosition:
    actuatorsInitState();
    break;
  case waitOnObjDetection:
    if (objIsDetected())
    {
      if (timeElapsed(_confirmTimeInterval))
        changeState(timedActuatorControl);
    }
    else
      changeState(waitOnObjDetection);
    break;
  case timedActuatorControl:
    if (timeElapsed(_actuationCycle))
      changeState(confirmObjRemoval);
    else
      setOutputs();
    break;
  case confirmObjRemoval:
    if (objIsRemoved())
    {
      if (timeElapsed(_confirmTimeInterval))
        changeState(waitOnObjDetection);
    }
    else
      changeState(confirmObjRemoval);
    break;
  case inactiveState:
    break;
  default:
    changeState(waitOnObjDetection);
    break;
  }
}