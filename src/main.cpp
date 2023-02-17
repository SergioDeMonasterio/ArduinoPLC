#include <Arduino.h>
#include "../lib/board_config/board_config.h"
#include "../lib/board_config/system_constants.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/crimper_control/crimper_control.h"
#include "../lib/pt78_insert_control/pt78_insert_control.h"
#include "../lib/tube_feeder_control/tube_feeder_control.h"

CrimperCtrlFSM
    crimper_1 = CrimperCtrlFSM(inPins[0],  // Inductive sensor pin
                               LOW,        // Sensor Detects = LOW
                               outPins[0], // crimper valve pin
                               outPins[1], // tube insert valve pin
                               outPins[2], // plug tip feeder valve pin
                               false,      // Crimping operation active = true/false
                               250,        // Sensor confirmation time interval
                               550,        // Insert valve on delay
                               250,        // Crimper valve on delay
                               250,        // Crimper valve off delay
                               300);       // Insert valve off delay

CrimperCtrlFSM
    crimper_2 = CrimperCtrlFSM(inPins[1],  // Inductive sensor pin
                               HIGH,       // Sensor Detects = LOW
                               outPins[3], // crimper valve pin
                               outPins[4], // tube insert valve pin
                               outPins[5], // plug tip feeder valve pin
                               true,       // Crimping operation active = true/false
                               250,        // Sensor confirmation time interval
                               550,        // Insert valve on delay
                               250,        // Crimper valve on delay
                               250,        // Crimper valve off delay
                               300);       // Insert valve off delay

// TubeFeederCtrlFSM tubeFeeder = TubeFeederCtrlFSM(inPins[0],
//                                                  outPins[0],
//                                                  outPins[1],
//                                                  outPins[2]);

// PT78_InsertCtrlFSM
//     pt78_inserter = PT78_InsertCtrlFSM(inPins[1],     // Inductive sensor pin
//                                LOW,           // Sensor Detects = LOW
//                                outPins[3],    // crimper valve pin
//                                outPins[4],    // tube insert valve pin
//                                outPins[5],    // plug tip feeder valve pin
//                                true,         // Crimping operation active = true/false
//                                400,           // Sensor confirmation time interval
//                                450,           // Insert valve on delay
//                                250,           // Crimper valve on delay
//                                550,           // Crimper valve off delay
//                                300);          // Insert valve off delay

void setup()
{
  Serial.begin(9600);
#if BOARD_CHOICE == 1
  Serial.println("Arduino NANO Board in use");
#elif BOARD_CHOICE == 2
  Serial.println("Arduino MEGA Board in use");
#endif
  configAllPins();
  delay(1000);
  crimper_1.start();
  crimper_2.start();
  // pt78_inserter.start();
  // tubeFeeder.start();
}

void loop()
{

  crimper_1.run();
  crimper_2.run();

  // pt78_inserter.run();
  // tubeFeeder.run();

  // ------> TESTs <--------
  //// ---> Test 1
  // connect all input pins with the respective output pins
  // for (int i = 2; i < outPinQty; i++)
  // {
  //   connectPins(inPins[i], outPins[i]);
  //   digitalWrite(outPins[i], HIGH);
  // }
  //// ---> End: Test 1
  // // TEST 2: Air Valves
  // if (digitalRead(inPins[1]) == LOW) digitalWrite(outPins[3], FEEDER_LIFT_TUBE);
  // else digitalWrite(outPins[3], FEEDER_DOWN_TUBE);
  // // ----> TETS: Feeder Init Position <-----
  // digitalWrite(outPins[3], FEEDER_ARM_MOVE_IN);
  // digitalWrite(outPins[4], FEEDER_DOWN_TUBE);
  // digitalWrite(outPins[5], FEEDER_HORIZONTAL_INIT_POS);
  // // ----> END TET 2: Feeder Init Position <-----
}
