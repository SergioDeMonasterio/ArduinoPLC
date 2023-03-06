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
  char _unitName[20];             // Unit name for logging max. 20 chars
  boolean _sensorDrivenOperation; // Sensor detection is activation the operation? TRUE / FALSE
  uint8_t _inPinSensor;           // Sensor input pin: Please use in pins defined in board config file
  uint8_t _sensorDetectsOn;       // Sensor detects on: HIGH / LOW depending on its type: PNP / NPN
  uint16_t _confirmTimeInterval;  // Sensor detection confirm time interval
  uint16_t _actuationCycle;       // full actuation cycle time: after this time has elapsed the machine goes to a new detection cycle
  uint8_t _actuatorsQty;          // Quantity of all simple (ditial outputs) actuators
  unsigned long _lastChangeTime;  // the time stamp of the last machine state change

  SACStates _currentState; // Current machine state

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
  actuatorPinsAndTimes _actOutPinsAndSwitchTimes[];

public:
  SimpleActuatorControl(
      char unitName[20],             // Unit name for logging max. 20 chars
      boolean sensorDrivenOperation, // Sensor detection is activation the operation? TRUE / FALSE
      uint8_t inPinSensor,           // Sensor input pin: Please use in pins defined in board config file
      uint8_t sensorDetectsOn,       // Sensor detects on: HIGH / LOW depending on its type: PNP / NPN
      uint16_t confirmTimeInterval,  // Sensor detection confirm time interval
      uint16_t actuationCycle,       // Full actuation cycle time: after this time has elapsed the machine goes to a new detection cycle
      uint8_t actuatorsQty,          // Quantity of all simple (ditial outputs) actuators
      // Array of all actuator digital output pins start- and stop- times
      //  Actuator On/Off,  BoardPin,  ____|--------------|______
      // {0/1,              outPin[0], startActTime, stopActTime}
      actuatorPinsAndTimes actOutPinsAndSwitchTimes[]);

  // Brings the unit into the initial position
  void start();
  // Starts the actuator action
  unsigned int run();
  // Stops the actuator action
  void stop();
};

#endif