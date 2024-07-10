#include "Led.h"

Led::Led(uint8_t pin) 
: pin(pin) {
  pinMode(pin, OUTPUT);
}

void Led::on(int delaySeconds) {
  digitalWrite(pin, HIGH);
  delay(delaySeconds);
  digitalWrite(pin, LOW);
}

void Led::off() {
  digitalWrite(pin, LOW);
}