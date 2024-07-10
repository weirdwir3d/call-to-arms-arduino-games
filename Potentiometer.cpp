#include "Potentiometer.h"

Potentiometer::Potentiometer(uint8_t pin) 
: pin(pin) {
  pinMode(pin, INPUT);
}