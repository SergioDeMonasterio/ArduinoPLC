#ifndef SIMPLE_ACTUATOR_CONTROL_H
#define SIMPLE_ACTUATOR_CONTROL_H

#include <Arduino.h>

enum SACStates
{
  initPosition = 0,
  waitOnObjDetection = 1,
  timedActuatorControl = 2,
  confirmObjRemoval = 3,
  inactiveState = 4
};

extern char *SACStatesStr[];

// {Actuator On/Off, BoardPins, startActTimes, stopActTimes}
// {TRUE/FALSE,      outPin[0], ____|--------------|______ } 1st actuator valve
// {TRUE/FALSE,      outPin[1], _________|--------------|_ } 2nd actuator valve
// {TRUE/FALSE,      outPin[2], ____________|----|________ } 3rd actuator valve ...
struct actuatorPinsAndTimes
{
  bool actActive;
  uint8_t boardPin;
  uint16_t startActTime;
  uint16_t stopActTime;
};

// Sensor detection config:
// {objDetConfirm: TRUE/FALSE, objRemovedConfirm: TRUE/FALSE, SensInPin: #Pin, SensDetectsOn: HIGH/LOW, DebounceTime}
struct sensorOperationConfig
{
  bool objDetectedConfirmation;
  bool objRemovedConfirmation;
  uint8_t sensorInPin;
  uint8_t sensorDetectsOn;
  uint16_t confirmTimeInterval;
};
// SimpleActuatorControl controls simple digital outputs according to the defined timings
// It has also a sensor input pin for sensor driven operation if needed.
// In case of sensor driven operation: the unit starts the operation after detecting an object.
// Otherwise it would start according to the defined timings.
// The Interface is defined as:
// SimpleActuatorControl(char unitName[20],                     // Actuation unit name
//                       sensorDrivenOperation = TRUE/FALSE,    // A sensor activates the unit operation?
//                       inPinSensor           = sensorInPin,   // Sensoir input pin number
//                       sensorDetectsOn       = HIGH/LOW,      // PNP sensor produces HIGH on detection, NPN - LOW
//                       confirmTimeInterval   = 350 ,          // Sensor debounce time in [ms]
//                       actuationCycle        = 1300,          // Full actuation cycle time in [ms]
//                       actuatorsQty           = 3,            // Number of actuators in the unit (air cylinders for example)
//                       actOutPinsAndSwitchTimes[] )           // Actuator timing matrix
//
// Actuator timing matrix "Actuator timing matrix" is defined as:
//            Actuator On/Off,  BoardPin,  ____|--------------|______
//            {0/1,             outPin[0], startActTime, stopActTime} - 1st actuator (piston)
//            ...                                                     - nth actuator
class SimpleActuatorControl
{
private:
  String _unitName; // Unit name for logging max. 20 chars

  sensorOperationConfig _sensorConfig;
  uint16_t _actuationCycle;      // full actuation cycle time: after this time has elapsed the machine goes to a new detection cycle
  uint8_t _actuatorsQty;         // Quantity of all simple (ditial outputs) actuators
  unsigned long _lastChangeTime; // the time stamp of the last machine state change

  SACStates _currentState; // Current machine state
  unsigned int _objCounter;
  // Moves all the actuators in their init state: all outPins => LOW
  void actuatorsInitState();
  // returns TRUE if an object is detected all the time during confirmTimeInterval
  boolean objIsDetected();
  // returns TRUE if an object has been removed during all the time of confirmTimeInterval
  boolean objIsRemoved();
  // gets the time interval since the last machine state change
  unsigned long getTimeInterval();
  // returns TRUE if the defined time interval has elapsed since the last machine state change
  boolean timeElapsed(unsigned int time);
  // Set the outputs according to the defined timings
  void setOutputs();
  // changes the machine state to the next one
  void changeState(SACStates nextState);
  // Array of all actuator digital output pins start- and stop- times
  //  Actuator On/Off,  BoardPin,  ____|--------------|______
  // {0/1,              outPin[0], startActTime, stopActTime}
  actuatorPinsAndTimes _actOutPinsAndSwitchTimes[10];

public:
  SimpleActuatorControl(
      String unitName, // Unit name for logging max. 20 chars
      // Sensor detection config:
      // {objDetConfirm: TRUE/FALSE, objRemovedConfirm: TRUE/FALSE, SensInPin: #Pin, SensDetectsOn: HIGH/LOW, DebounceTime}
      sensorOperationConfig sensorConfig,
      uint16_t actuationCycle, // Full actuation cycle time: after this time has elapsed the machine goes to a new detection cycle
      uint8_t actuatorsQty,    // Quantity of all simple (ditial outputs) actuators
      // Array of all actuator digital output pins start- and stop- times
      //  Actuator On/Off,  BoardPin,  ____|--------------|______
      // {0/1,              outPin[0], startActTime, stopActTime}
      actuatorPinsAndTimes actOutPinsAndSwitchTimes[10]);

  // Brings the unit into the initial position
  void start();
  // Starts the actuator action
  unsigned int run();
  // Stops the actuator action
  void stop();
  // Moving to init position
  void moveToInitPos();
  // Returns Machine State
  SACStates getMachineState();
};

#endif