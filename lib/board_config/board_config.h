#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <Arduino.h>

// // -----------> Board Pins Config: PLC with Mega  <-----------

// // Inputs
// static const uint8_t inPinQty = 14; // Qty of input pins
// static const uint8_t inPins[inPinQty] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

// // Outputs
// static const uint8_t outPinQty = 14; // Qty of output pins
// static const uint8_t outPins[outPinQty] = {42, 41, 40, 5, 4, 3, 2, 49, 48, 47, 46, 45, 44, 43};

// // -----------> End of: PLC with Mega <----------------------
// -----------> Board Pins Config: PLC with Nano <-----------

// Digital Inputs
static const uint8_t inPinQty = 4;                      // Qty of input pins
static const uint8_t inPins[inPinQty] = {7, 8, 12, 13}; // Input Pins 1..4

// Relay Outputs
static const uint8_t outPinQty = 5;                         // Qty of output pins
static const uint8_t outPins[outPinQty] = {2, 3, 4, 5, 16}; //,23 - buggy output pin // Output Pins 1..6
// -----------> End of: PLC with Nano <----------------------

void pinConfig(const uint8_t inPinArray[], const uint8_t pinArrayLength, const int pinType);
void configAllPins();

#endif