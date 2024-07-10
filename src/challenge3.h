#ifndef CHALLENGE3_H
#define CHALLENGE3_H

#include <LiquidCrystal_I2C.h>
#include "tm1638p.h"
#include "Buzzer.h"

extern LiquidCrystal_I2C lcd;
extern Buzzer buzzer;
extern TM1638plus tm;

extern int points;
extern unsigned long startTime;

void challenge3();
void whackamoleSequence();
void lightningSequence();
void displayRound(int roundNr, int delayVal);

#endif 