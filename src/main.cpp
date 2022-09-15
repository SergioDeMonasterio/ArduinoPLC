#include <Arduino.h>
#include "../lib/air_cylinder_control/air_cylinder_control.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/board_config/board_config.h"
// Crimping-Tool Object Detection

unsigned long currentTime;
CrimperCtrlFSM
    crimper_1 = CrimperCtrlFSM(    inPins[0],
                                   outPins[0],
                                   outPins[1],
                                   300,
                                   150,
                                   350,
                                   200,
                                   150,
                                   400);

// AirCylinderCtrlFSM
//     crimper_2 = AirCylinderCtrlFSM(inPins[1],
//                                    outPins[1],
//                                    300,
//                                    500,
//                                    1000);

void setup()
{

  Serial.begin(9600);
  configAllPins();
  delay(1000);
  currentTime = millis();
  Serial.print("Current time: ");
  Serial.println(currentTime);
  crimper_1.start(currentTime);
  // crimper_2.start(currentTime);
}

void loop()
{
  // connect all input pins with the respective output pins
  // for (int i = 1; i < inPinQty; i++)
  // {
  //   connectPins(inPins[i], outPins[i]);
  // }
  currentTime = millis();
  crimper_1.run(currentTime);
  // crimper_2.run(currentTime);
}
