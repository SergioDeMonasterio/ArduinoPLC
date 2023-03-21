#include "board_config.h"

// IO Pin Config Function
void pinConfig(const uint8_t inPinArray[], const uint8_t pinArrayLength, const int pinType)
{
  for (int i = 0; i < pinArrayLength; i++)
  {
    pinMode(inPinArray[i], pinType);
  }
}

void configAllPins()
{
  pinConfig(inPins, inPinQty, INPUT_PULLUP);
  pinConfig(outPins, outPinQty, OUTPUT);
}
