#ifndef CHALLENGE2_H
#define CHALLENGE2_H

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

void challenge2();
void playRound(int questionNr, int time, int delayVal);
void displayFeedback(int questionNr, int delayVal);
void showOptions(int value);
void displayQuestion(int questionNr, int delayVal);
void blinkNumber(String number, int col, int row);
void feedbackCorrectAnswer(int questionNr);
void feedbackWrongAnswer();
void promptForAnswer();

#endif 