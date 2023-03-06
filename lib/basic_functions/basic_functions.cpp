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

// Object Detection
bool objDetected(uint8_t inPinSensor, uint8_t detectionValue)
{
  return not(digitalRead(inPinSensor) == detectionValue); // Leight copplers invert the signal!
}

// Get the time interval starting with "fromTimeStamp" to "now"
unsigned long getTimeInterval(unsigned long fromTimeStamp)
{
  unsigned long timeInterval = millis() - fromTimeStamp;
  return timeInterval;
}

// Return TRUE if the "time" has elapsed conting "fromTimeStamp"
bool timeElapsed(unsigned long fromTimeStamp, unsigned int time)
{
  return getTimeInterval(fromTimeStamp) > time;
}

// Coppies the array: copyarr = arr
void copyArray(int copyarr[], int arr[], size_t size)
{
  for (short i = 0; i < size; ++i)
  {
    copyarr[i] = arr[i];
  }
}