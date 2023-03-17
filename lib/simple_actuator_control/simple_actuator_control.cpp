#include <Arduino.h>
#include "simple_actuator_control.h"
#include "../basic_functions/basic_functions.h"

char *SACStatesStr[] = {"initPosition",
                        "waitOnObjDetection",
                        "timedActuatorControl",
                        "confirmObjRemoval",
                        "inactiveState"};

SimpleActuatorControl::SimpleActuatorControl(String unitName,                                   // Unit name for logging max. 20 chars
                                             sensorOperationConfig sensorConfig,                // Sensor driven operation configuration
                                             uint16_t actuationCycle,                           // Full actuation cycle time
                                             uint8_t actuatorsQty,                              // Quantity of all simple (ditial outputs) actuators
                                             actuatorPinsAndTimes actOutPinsAndSwitchTimes[10]) // Output timing matrix
{
  _unitName = unitName;
  _sensorConfig = sensorConfig;
  _actuationCycle = actuationCycle;
  _actuatorsQty = actuatorsQty;
  // Copy the actuators pins and time matrix to the private variable (unchangable after initiation)
  for (short i = 0; i < actuatorsQty; ++i)
    _actOutPinsAndSwitchTimes[i] = actOutPinsAndSwitchTimes[i];

  _currentState = initPosition;
  _objCounter = 0;
}

// Moves all the actuators in their init state: all outPins => LOW
void SimpleActuatorControl::actuatorsInitState()
{
  for (short i = 0; i < _actuatorsQty; ++i)
    digitalWrite(_actOutPinsAndSwitchTimes[i].boardPin, LOW);
}

boolean SimpleActuatorControl::objIsDetected()
{
  return objDetected(_sensorConfig.sensorInPin, _sensorConfig.sensorDetectsOn);
}

boolean SimpleActuatorControl::objIsRemoved()
{
  return not objDetected(_sensorConfig.sensorInPin, _sensorConfig.sensorDetectsOn);
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
    Serial.print(_unitName + " State: ");
    Serial.println(SACStatesStr[nextState]);
  }
  _currentState = nextState;
  _lastChangeTime = millis();
}

void SimpleActuatorControl::start()
{
  actuatorsInitState(); // Moving to init position
  delay(100);
  Serial.println(_unitName + " STARTED!");
  changeState(waitOnObjDetection);
}

void SimpleActuatorControl::stop()
{
  Serial.println(_unitName + " STOPPED!");
  changeState(inactiveState);
}

void SimpleActuatorControl::moveToInitPos()
{
  Serial.println(_unitName + " MOVING TO INIT POSITION!");
  changeState(initPosition);
}

SACStates SimpleActuatorControl::getMachineState()
{
  return _currentState;
}

unsigned int SimpleActuatorControl::run()
{
  switch (_currentState)
  {
  case initPosition:
    actuatorsInitState();
    break;
  case waitOnObjDetection:
    if (_sensorConfig.objDetectedConfirmation)
      if (objIsDetected())
      {
        if (timeElapsed(_sensorConfig.confirmTimeInterval))
        {
          changeState(timedActuatorControl);
          _objCounter++;
        }
      }
      else
        changeState(waitOnObjDetection);
    else
      changeState(timedActuatorControl);
    break;
  case timedActuatorControl:
    if (timeElapsed(_actuationCycle))
      changeState(confirmObjRemoval);
    else
      setOutputs();
    break;
  case confirmObjRemoval:
    if (_sensorConfig.objRemovedConfirmation)
      if (objIsRemoved())
      {
        if (timeElapsed(_sensorConfig.confirmTimeInterval))
          changeState(waitOnObjDetection);
      }
      else
        changeState(confirmObjRemoval);
    else
      changeState(waitOnObjDetection);
    break;
  case inactiveState:
    break;
  default:
    changeState(waitOnObjDetection);
    break;
  }
  return _objCounter;
}