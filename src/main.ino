#include <Arduino.h>
#include "Buzzer.h"
#include "tm1638p.h"
#include "Potentiometer.h"
#include "UserBtn.h"
#include <LiquidCrystal_I2C.h>

#include "challenge1.h"
#include "challenge2.h"
#include "challenge3.h"
#include "challenge4.h"

#define BUZZER_PIN 5
Buzzer buzzer(BUZZER_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define POTENTIOMETER_PIN A5
Potentiometer potentiometer(POTENTIOMETER_PIN);
UserBtn userBtn;

#define PIN_TM_STB 10
#define PIN_TM_CLK 8
#define PIN_TM_DIO 9
TM1638plus tm(PIN_TM_STB, PIN_TM_CLK, PIN_TM_DIO);

unsigned long startTime;
int points = 0;

void setup() {
  // create the possibility for the program to randomize numbers
  randomSeed(analogRead(A0));
  // Serial.begin(9600); uncomment for debugging
  displayPoints();
}

void loop() {

  // challenge1();
  // challenge2();
  // challenge3();
  challenge4();

  lcd.clear();
  if (points < 100) {
    lcd.setCursor(0, 0);
    lcd.print("You did not pass");
    lcd.setCursor(0, 1);
    lcd.print("test");
    buzzer.playLoseSound();
    delay(5000);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("You passed the");
    lcd.setCursor(0, 1);
    lcd.print("test!!!");
    buzzer.rickroll();
  }
  exit(1);
}

int* generateRandomSequence(int* array, int nrOfBlinks, int lowerBound, int higherBound) {
  for (int i = 0; i < nrOfBlinks; i++) {
    int rand = random(lowerBound, higherBound);
    array[i] = rand;
  }
  int n = sizeof(array) / sizeof(array[0]);

  return array;
}

void startTimer() {
  startTime = millis();
}

void displayPoints() {
  tm.displayBegin();
  //display correctly numbers with one, two or three characters (like 1, 10 and 100)
  int hundreds = (points / 100) % 10;
  int tens = (points / 10) % 10;
  int unit = points % 10;

  if (hundreds > 0) {
    tm.displayHex(4, hundreds);
    tm.displayHex(5, tens);
    tm.displayHex(6, unit);
  } else if (tens > 0) {
    tm.displayHex(5, tens);
    tm.displayHex(6, unit);
  } else {
    tm.displayHex(6, unit);
  }
}

void startChallengeMessage(int challengeNr) {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Welcome to");
  lcd.setCursor(1, 1);
  lcd.print("challenge");
  lcd.setCursor(11, 1);
  lcd.print(challengeNr);
  delay(5000);
}

void doneChallengeMessage(int challengeNr) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("That's it for");
  lcd.setCursor(1, 1);
  lcd.print("challenge");
  lcd.setCursor(11, 1);
  lcd.print(challengeNr);
  delay(5000);
}

void displayRoundMessage(int roundNr, int delayVal) {
  lcd.clear();
  if (roundNr == 1) {
    lcd.setCursor(6, 0);
    lcd.print("FIRST");
    lcd.setCursor(6, 1);
    lcd.print("ROUND");
  } else if (roundNr == 2) {
    lcd.setCursor(4, 0);
    lcd.print("SECOND");
    lcd.setCursor(6, 1);
    lcd.print("ROUND");
  } else {
    lcd.setCursor(4, 0);
    lcd.print("THIRD");
    lcd.setCursor(6, 1);
    lcd.print("ROUND");
  }
  delay(delayVal);
  lcd.clear();
}

int findBtnPressed(int binaryNr) {
  //find out what button of the tm board is being pressed
  if (binaryNr == 1) {
    // Serial.println("Button 1 pressed");
    return 1;
  } else if (binaryNr == 2) {
    // Serial.println("Button 2 pressed");
    return 2;
  } else if (binaryNr == 4) {
    // Serial.println("Button 3 pressed");
    return 3;
  } else if (binaryNr == 8) {
    // Serial.println("Button 4 pressed");
    return 4;
  } else if (binaryNr == 16) {
    // Serial.println("Button 5 pressed");
    return 5;
  } else if (binaryNr == 32) {
    // Serial.println("Button 6 pressed");
    return 6;
  } else if (binaryNr == 64) {
    // Serial.println("Button 7 pressed");
    return 7;
  } else if (binaryNr == 128) {
    // Serial.println("Button 8 pressed");
    return 8;
  }
  return -1;
}
