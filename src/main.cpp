#include <Arduino.h>
#include "../lib/board_config/board_config.h"
#include "../lib/board_config/system_constants.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/simple_actuator_control/simple_actuator_control.h"

// -------------------< Tube Feeder >------------------------------------------------------------------------
// 1. Sensor driven operation {true, false, inPins[8], HIGH, 250}:
//    a) if tubes available => start the cycle and get the next tube -TRUE
//    b) no need in confirmation if the tube has been removed to finish the cycle - FALSE
//    c) Input pin - inPins[8], sens detects on HIGH, debounce time 250 ms
sensorOperationConfig tubeFeeder_sensConfig = {true, false, inPins[8], HIGH, 200};
// 2. actuatorPinsAndTimes:
// {On/Off, BoardPins, startActTimes, stopActTimes} ENGLISH           / ESPAÑOL
// {TRUE,   outPin[3], _|----------------|_______ } Tube feeder arm   / Piston para sacer el tubo de la caja
// {TRUE,   outPin[4], _______|----------|_______ } Tube lifter arm   / Piston para para levantar y bajar el tubo
// {TRUE,   outPin[5], _____________|----------|_ } Horizontal move   / Piston para mover los tubos a lo largo de la mesa
//                      |     |     |    |     |
//                  -------------------------------> t, [ms]
//                      1    800  1300  2600  3100
const short tubeFeeder_actQty = 3; // Number of actuators
actuatorPinsAndTimes
    tubeFeeder_PinsAndSwitchTimes[tubeFeeder_actQty] = {{true, outPins[3], 1, 2600},     // Tube feeder arm
                                                        {true, outPins[4], 800, 2600},   // Tube lifter arm
                                                        {true, outPins[5], 1300, 3100}}; // Horizontal moving valve
SimpleActuatorControl tubeFeeder = SimpleActuatorControl("Tube Feeder",                  // Unit name
                                                         tubeFeeder_sensConfig,          // Sensor operation configuration
                                                         4400,                           // Total cycle time 3200 ms
                                                         tubeFeeder_actQty,              // Number of actuators / cylinders
                                                         tubeFeeder_PinsAndSwitchTimes); // Timing matrix
// -------------------< End of: Tube Feeder  >---------------------------------------------------------------

// // -------------------< 7/8" Crimper >------------------------------------------------------------------------
// // 1. actuatorPinsAndTimes:
// // {On/Off, BoardPins, startActTimes, stopActTimes} ENGLISH           / ESPAÑOL
// // {TRUE,   outPin[3], _|----------------------|_ } Hold tube valve   / Piston para aplastar el tubo
// // {TRUE,   outPin[4], _______|----------------|_ } Tube Insert valve / Piston para mover el tubo
// // {TRUE,   outPin[5], _____________|----------|_ } PT Feeder valve   / Piston para insertar el plug tip
// //                      |     |     |    |     |
// //                  -------------------------------> t, [ms]
// //                      1    350   600  1050  1350
// const short crimper_78_actQty = 3; // Number of actuators
// actuatorPinsAndTimes
//     crimper_78_PinsAndSwitchTimes[crimper_78_actQty] = {{true, outPins[3], 1, 1350},     // Hold tube valve
//                                                         {true, outPins[4], 350, 1350},   // Tube Insert valve
//                                                         {true, outPins[5], 600, 1350}};  // PT Feeder valve
// SimpleActuatorControl crimper_78 = SimpleActuatorControl("7/8 Crimper",                  // Unit name
//                                                          true,                           // Sensor driven operation
//                                                          inPins[1],                      // Sensor input pin
//                                                          HIGH,                           // Sensor active on HIGH
//                                                          250,                            // Detecton debounce time
//                                                          1400,                           // Total cycle time
//                                                          crimper_78_actQty,              // Number of actuators / cylinders
//                                                          crimper_78_PinsAndSwitchTimes); // Timing matrix
// // -------------------< End of: 7/8" Crimper  >---------------------------------------------------------------

// -------------------< 3/4" Crimper >------------------------------------------------------------------------
unsigned int crimped_34_tips_cnt = 0;
unsigned int crimped_34_tips_cnt_prev = 0;
// 1. Sensor driven operation: obj detect- and obj removed confirmation
sensorOperationConfig crimper_34_sensConfig = {true, true, inPins[2], HIGH, 250};
const short crimper_34_actQty = 3; // Number of actuators
// 2. actuatorPinsAndTimes:
// {On/Off, BoardPins, startActTimes, stopActTimes} ENGLISH           / ESPAÑOL
// {TRUE,   outPin[2], _|----------------------|_ } PT Feeder valve   / Piston para insertar el plug tip
// {TRUE,   outPin[1], _______|----------------|_ } Tube Insert valve / Piston para mover el tubo
// {TRUE,   outPin[0], _____________|----|_______ } Crimping valve    / Piston para remachar
//                      |     |     |    |     |
//                  -------------------------------> t, [ms]
//                      1    550   800  1050  1350
actuatorPinsAndTimes
    crimper_34_PinsAndSwitchTimes[crimper_34_actQty] = {{true, outPins[8], 1, 1350},     // PT Feeder valve
                                                        {true, outPins[7], 550, 1350},   // Tube Insert valve
                                                        {true, outPins[6], 800, 1050}};  // Crimping valve
SimpleActuatorControl crimper_34 = SimpleActuatorControl("3/4 Crimper",                  // Unit name
                                                         crimper_34_sensConfig,          // Sensor operation configuration
                                                         1400,                           // Total cycle time
                                                         crimper_34_actQty,              // Number of actuators / cylinders
                                                         crimper_34_PinsAndSwitchTimes); // Timing matrix
// -------------------< End of: 3/4" Crimper  >---------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
#if BOARD_CHOICE == 1
  Serial.println("Arduino NANO Board in use");
#elif BOARD_CHOICE == 2
  Serial.println("Arduino MEGA Board in use");
#endif
  configAllPins();
  delay(100);

  tubeFeeder.start();
  // crimper_78.start();
  crimper_34.start();
  delay(1500);
}

void loop()
{
  // Main machine breaker: AT THE MOMENT JUST STOP THE FEEDER!
  if (digitalRead(inPins[5]) == LOW)
  {
    Serial.println("Breaker activated !!!!!");
    // Stop all units movements
    // crimper_34.stop();
    tubeFeeder.stop();
  }
  else if (tubeFeeder.getMachineState() == inactiveState) // || crimper_34.getMachineState() == inactiveState
  {
    // Restart all units
    // crimper_34.start();
    tubeFeeder.start();
    delay(1500);
  }
  else
  {
    tubeFeeder.run();
  }

  crimped_34_tips_cnt = crimper_34.run();
  if (crimped_34_tips_cnt_prev != crimped_34_tips_cnt)
  {
    crimped_34_tips_cnt_prev = crimped_34_tips_cnt;
    Serial.print("Crimped 3/4 tips: ");
    Serial.println(crimped_34_tips_cnt);
  }

  // ------> TESTs <--------
  //// ---> Test 1
  // connect all input pins with the respective output pins
  // for (int i = 2; i < outPinQty; i++)
  // {
  //   connectPins(inPins[i], outPins[i]);
  //   digitalWrite(outPins[i], HIGH);
  // }
  //// ---> End: Test 1
}
