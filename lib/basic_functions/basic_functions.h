#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H

#include <Arduino.h>

// Connects pins
void connectPins(uint8_t inPin, uint8_t outPin);
// Object Detection
bool objDetected(uint8_t inPinSensor, uint8_t detectionValue);
// Get the time interval starting with "fromTimeStamp" to "now"
unsigned long getTimeInterval(unsigned long fromTimeStamp);
// Return TRUE if the "time" has elapsed conting "fromTimeStamp"
bool timeElapsed(unsigned long fromTimeStamp, unsigned int time);
// Coppies the array: copyarr = arr
void copyArray(int copyarr[], int arr[], size_t size);

#endif