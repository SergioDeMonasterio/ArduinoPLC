#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H

#include <Arduino.h>
// Connects pins
void connectPins(uint8_t inPin, uint8_t outPin);
// Object Detection
bool objDetected(uint8_t inPinSensor, uint8_t detectionValue);
#endif