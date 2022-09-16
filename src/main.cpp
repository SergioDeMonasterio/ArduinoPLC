#include <Arduino.h>
#include "../lib/air_cylinder_control/air_cylinder_control.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/board_config/board_config.h"
// Crimping-Tool Object Detection

AirCylinderCtrlFSM
    crimper_1 = AirCylinderCtrlFSM(inPins[0],
                                   outPins[0],
                                   outPins[1],
                                   300,
                                   150,
                                   150,
                                   150,
                                   200);

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
  crimper_1.start();

}

void loop()
{
  // connect all input pins with the respective output pins
  // for (int i = 1; i < inPinQty; i++)
  // {
  //   connectPins(inPins[i], outPins[i]);
  // }

  crimper_1.run();
}
