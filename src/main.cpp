#include <Arduino.h>
#include "../lib/board_config/board_config.h"
#include "../lib/board_config/system_constants.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/crimper_control/crimper_control.h"
#include "../lib/pt78_insert_control/pt78_insert_control.h"
#include "../lib/tube_feeder_control/tube_feeder_control.h"


TubeFeederCtrlFSM tubeFeeder = TubeFeederCtrlFSM(inPins[0],   // Sensor de la caja con tubos: Todavia hay tubos en la caja?
                                                 HIGH,        // / Sensor Detects = HIGH
                                                 inPins[3],   // Tube feeder breaker!!     
                                                 outPins[0],  // Cilindro que saca tubo por tubo de la caja
                                                 outPins[1],  // Cilindro que levanta los tubos
                                                 outPins[2]); // Cilindro largo que mueve los tubos horizontal

PT78_InsertCtrlFSM
    pt78_inserter = PT78_InsertCtrlFSM(inPins[1],     // Sensor inductivo donde se insertan los plasticos de 7/8"
                               LOW,                   // Sensor Detects = LOW
                               outPins[3],            // Cilindro que remacha / crimper valve pin
                               outPins[4],            // Cilindro que mueve el tubo / tube insert valve pin
                               outPins[5],            // Cilindro que inserta los plug tips / plug tip feeder valve pin
                               true,         // Crimping operation active = true/false
                               300,           // Sensor confirmation time interval
                               350,           // Insert valve on delay
                               250,           // Crimper valve on delay
                               450,           // Crimper valve off delay
                               300);          // Insert valve off delay

CrimperCtrlFSM
    crimper_34 = CrimperCtrlFSM(inPins[2],     // Inductive sensor pin
                               HIGH,           // Sensor Detects = HIGH
                               outPins[6],    // Piston para remachar / crimper valve pin
                               outPins[7],    // Piston para mover el tubo /  tube insert valve pin
                               outPins[8],    // Piston para insertar el plug tip / plug tip feeder valve pin
                               true,         // Crimping operation active = true/false
                               250,           // Sensor confirmation time interval
                               550,           // Insert valve on delay
                               250,           // Crimper valve on delay
                               250,           // Crimper valve off delay
                               300);          // Insert valve off delay


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
  
  pt78_inserter.start();
  tubeFeeder.start();
  crimper_34.start();
}

void loop()
{

  pt78_inserter.run();
  tubeFeeder.run();
  crimper_34.run();

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
