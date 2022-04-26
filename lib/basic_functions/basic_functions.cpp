#include "basic_functions.h"
#include <Arduino.h>

// Mirror the inPin & outPin for sensor testing
void connectPins(uint8_t inPin, uint8_t outPin)
{
  if (digitalRead(inPin) == LOW)
  {
    digitalWrite(outPin, HIGH);
  }
  else
  {
    digitalWrite(outPin, LOW);
  }
}
