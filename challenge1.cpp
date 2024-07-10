#include "challenge1.h"

void startChallengeMessage(int challengeNr);
void displayRoundMessage(int roundNr, int delayVal);
void startTimer();
void displayPoints();
void doneChallengeMessage(int challengeNr);
int* generateRandomSequence(int* array, int nrOfBlinks, int lowerBound, int higherBound);

Led greenLed(GREEN_LED);
Led yellowLed(YELLOW_LED);
Led redLed(RED_LED);

int counter = 0;
int firstRoundInputs[4];
int secondRoundInputs[5];
int thirdRoundInputs[6];
bool isCorrect = true;

void challenge1() {
  const long intervalButton = 200;

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  startChallengeMessage(1);

  // create the possibility for the program to randomize numbers
  randomSeed(analogRead(A0));

  //random number between 2 (included) and 5 (excluded)
  int randNum = random(2, 5);

  delay(2500);

  for (int i = 1; i < 4; i++) {
    playRound(i);

    delay(2000);
  }

  doneChallengeMessage(1);
}

void playRound(int round) {

  int sequenceLength;
  switch (round) {
    case 1:
      sequenceLength = 4;
      break;
    case 2:
      sequenceLength = 5;
      break;
    case 3:
      sequenceLength = 6;
      break;
    default:
      sequenceLength = 0;
      break;
  }

  int* roundInputs;
  switch (round) {
    case 1:
      roundInputs = firstRoundInputs;
      break;
    case 2:
      roundInputs = secondRoundInputs;
      break;
    case 3:
      roundInputs = thirdRoundInputs;
      break;
    default:
      roundInputs = nullptr;
  }

  int roundSequence[sequenceLength];
  displayRoundMessage(round, 1000);
  playSequence(generateRandomSequence(roundSequence, sequenceLength, 2, 5), sequenceLength);

  askForSequence();

  startTimer();

  registerUserInput(sequenceLength, roundInputs);

  bool isCorrect = true;
  for (int i = 0; i < sequenceLength; i++) {
    if (roundSequence[i] != roundInputs[i]) {
      isCorrect = false;
      break;
    }
  }
  checkCorrectedness(round, isCorrect);
  lcd.clear();
}

void checkCorrectedness(int round, boolean isCorrect) {
  if (isCorrect) {
    if (round == 1) {
      points = points + 5;
    } else if (round == 2) {
      points = points + 10;
    } else if (round == 3) {
      points = points + 15;
    }
    lcd.init();
    lcd.backlight();
    lcd.setCursor(3, 0);
    lcd.print("CORRECT!");
    buzzer.playWinSound();
    displayPoints();
    delay(5000);
  } else {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(5, 0);
    lcd.print("WRONG!");
    buzzer.playLoseSound();
  }
}

//functions

void registerUserInput(int nrOfLights, int inputs[]) {
  int time;
  if (nrOfLights == 4) {
    time = 12;
  } else if (nrOfLights == 5) {
    time = 13;
  } else if (nrOfLights == 6) {
    time = 15;
  }
  unsigned long endTime = startTime + (time * 1000);
  counter = 0;
  while (counter < nrOfLights && (millis() < endTime)) {
    unsigned long remainingTime = (endTime - millis()) / 1000;
    int tens = remainingTime / 10;  
    int ones = remainingTime % 10; 
    tm.displayHex(0, tens);
    tm.displayHex(1, ones);

    uint8_t buttons = tm.readButtons();
    if (buttons & BTN_3) {
      Serial.print("RED IS PRESSED");
      inputs[counter] = 4;
      counter++;
      delay(800);  // delay after button press to prevent multiple registrations
    } else if (buttons & BTN_2) {
      Serial.print("YELLOW IS PRESSED");
      inputs[counter] = 3;
      counter++;
      delay(800); 
    } else if (buttons & BTN_1) {
      Serial.print("GREEN IS PRESSED");
      inputs[counter] = 2;
      counter++;
      delay(800);  
    }
    while (buttons) {
      Serial.print("Button still pressed"); 
      buttons = tm.readButtons();           
    }
  }
}

void askForSequence() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("What's the");
  lcd.setCursor(3, 1);
  lcd.print("sequence?");
}

void makeLightsBlink(int array[], int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    // Serial.println(char(array[i]));
    switch (static_cast<int>(array[i])) {
      case 2:
        greenLed.on(700);
        yellowLed.off();
        redLed.off();
        // Serial.println("GREEN IS ON");
        delay(250);
        break;
      case 3:
        yellowLed.on(700);
        greenLed.off();
        redLed.off();
        // Serial.println("YELLOW IS ON");
        delay(250);
        break;
      case 4:
        redLed.on(700);
        yellowLed.off();
        greenLed.off();
        // Serial.println("RED IS ON");
        delay(250);
        break;
    }
  }
  Serial.println("finished blinking");
}

//input array and play sequence based on that array
void playSequence(int array[], int arraySize) {
  int newArray[arraySize];
  for (int i = 0; i < arraySize; i++) {
    newArray[i] = array[i];
  }

  makeLightsBlink(newArray, arraySize);
}