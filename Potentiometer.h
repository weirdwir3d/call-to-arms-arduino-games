#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <Arduino.h>
#define POTENTIOMETER_PIN A0

class Potentiometer {
  private:
    uint8_t pin;

  public:
    Potentiometer(uint8_t pin);
};

#endif