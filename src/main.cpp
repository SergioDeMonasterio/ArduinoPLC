#include <Arduino.h>
#include "../lib/crimper_control/crimper_control.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/board_config/board_config.h"
// Crimping-Tool Object Detection

CrimperCtrlFSM
    crimper_1 = CrimperCtrlFSM(inPins[0],
                               outPins[0],
                               outPins[1],
                               300,
                               150,
                               150,
                               250,
                               200);

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
  for (int i = 2; i < outPinQty; i++)
  {
    //   connectPins(inPins[i], outPins[i]);
    digitalWrite(outPins[i], HIGH);
  }

  crimper_1.run();
}
