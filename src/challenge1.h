#ifndef CHALLENGE1_H
#define CHALLENGE1_H

#include <LiquidCrystal_I2C.h>
#include "tm1638p.h"
#include "Buzzer.h"
#include "Led.h"

#define BUZZER_PIN 5
#define GREEN_LED 2
#define YELLOW_LED 3
#define RED_LED 4

extern TM1638plus tm;
extern Buzzer buzzer;
extern Led greenLed;
extern Led yellowLed;
extern Led redLed;
extern LiquidCrystal_I2C lcd;

extern int points;
extern unsigned long startTime;

void challenge1();
void playRound(int round);
void checkCorrectedness(int round, bool isCorrect);
void registerUserInput(int nrOfLights, int inputs[]);
void askForSequence();
void makeLightsBlink(int array[], int arraySize);
void playSequence(int array[], int arraySize);

#endif
