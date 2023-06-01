#include <Arduino.h>
#include "../lib/system_config/board_config.h"
#include "../lib/basic_functions/basic_functions.h"
#include "../lib/system_config/system_config.h"
#include "../lib/simple_actuator_control/simple_actuator_control.h"

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
  crimper_78.start();
  crimper_34.start();
  delay(1500);
}

void loop()
{
  // Main machine breaker: AT THE MOMENT JUST STOP THE FEEDER!
  if (objDetected(machineBreakerInPin, LOW))
  {
    Serial.println("Breaker activated !!!!!");
    // Stop all units movements
    tubeFeeder.stop();
    // crimper_78.stop();
    // crimper_34.stop();
  }
  else if (tubeFeeder.getMachineState() == inactiveState) // || crimper_34.getMachineState() == inactiveState
  {
    // Restart all units
    tubeFeeder.start();
    // crimper_78.start();
    // crimper_34.start();

    delay(1500);
  }
  else
  {
    tubeFeeder.run();
  }
  crimper_78.run();
  crimped_34_cnt = crimper_34.run();
  if (crimped_34_cntPrev != crimped_34_cnt)
  {
    crimped_34_cntPrev = crimped_34_cnt;
    Serial.print("Crimped 3/4 tips: ");
    Serial.println(crimped_34_cnt);
  }
}
