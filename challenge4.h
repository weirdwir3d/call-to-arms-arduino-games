#ifndef CHALLENGE4_H
#define CHALLENGE4_H

#include <LiquidCrystal_I2C.h>
#include "tm1638p.h"
#include "Buzzer.h"
#include "UserBtn.h"
#include "Potentiometer.h"

extern LiquidCrystal_I2C lcd;
extern Buzzer buzzer;
extern TM1638plus tm;
extern Potentiometer potentiometer;
extern UserBtn userBtn;

extern int points;
extern unsigned long startTime;

void challenge4();
void playHand(int roundNr);
int findIndex(String array[], String element);
void addElement(int array[], int size, int valueToAdd);
void removeElement(int array[], int size, int valueToRemove);
bool elementExists(int array[], int size, int element);
bool isArrayEmpty(int arr[], int size);
void computerThinking();
void bettingQuestion();
void displayPointsOnLcd();

#endif 