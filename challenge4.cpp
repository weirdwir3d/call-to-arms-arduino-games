#include "WSerial.h"
#include "wiring_time.h"
#include <stdlib.h>
#include "challenge4.h"

//to print bytes on the lcd screen
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args, BYTE);
#endif

//-------exported methods-------
void startChallengeMessage(int challengeNr);
void displayRoundMessage(int roundNr, int delayVal);
void startTimer();
void displayPoints();
void doneChallengeMessage(int challengeNr);
int* generateRandomSequence(int* array, int nrOfBlinks, int lowerBound, int higherBound);
int findBtnPressed(int binaryNr);

//-------local vars
// symbols
uint8_t bell[8] = { 0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4 };
uint8_t note[8] = { 0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0 };
uint8_t heart[8] = { 0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0 };
uint8_t sun[8] = { 0b00100, 0b10101, 0b01110, 0b11111, 0b01110, 0b10101, 0b00100, 0b00000 };
uint8_t alien[8] = { B11111, B11111, B10101, B11111, B01110, B01110, B01010, B11011 };
//unused
uint8_t moon[8] = { 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000 };

const uint8_t symbolsArray[6][7] = {
  { 0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4 },
  { 0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0 },
  { 0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0 },
  { 0b00100, 0b10101, 0b01110, 0b11111, 0b01110, 0b10101, 0b00100 },
  { B11111, B11111, B10101, B11111, B01110, B01110, B11011 },
  { 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000 }
};

//keep track of the points the user wants to bet
int betPoints = 0;

//possible outcomes combinations
String outcomes[7] = {
  "Five of a kind",   // 0
  "Four of a kind",   // 1
  "Full house",       // 2
  "Three of a kind",  // 3
  "Two pairs",        // 4
  "One pair",         // 5
  "Nothing"           // 6
};

//for each outcome above, a number is associated to indicate its power
int outcomeScores[7] = { 7, 6, 5, 4, 3, 2, 0 };

//computer outcomes, to randomize. You can tweak it to edit the outcomes probability
// "Two pairs" (4) and "Three of a kind" (3) seem to be the most likely ones, so we will add more instances of them in the array below, and so on...
int probabilityArray[28] = { 4, 4, 4, 4, 4, 4, 4,
                             3, 3, 3, 3, 3, 3,
                             5, 5, 5, 5, 5,
                             2, 2, 2, 2,
                             1, 1, 1,
                             0, 0,
                             6 };

void challenge4() {

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  tm.displayBegin();

  //----------SETUP----------
  lcd.createChar(1, bell);
  lcd.createChar(2, note);
  lcd.createChar(3, heart);
  lcd.createChar(4, sun);
  lcd.createChar(5, alien);
  //unused
  lcd.createChar(0, moon);
  //----------END SETUP----------

  startChallengeMessage(4);

  for (int i = 1; i < 5; i++) {
    playHand(i);

    delay(5000);
  }

  doneChallengeMessage(4);
}

//functions
void playHand(int roundNr) {
  displayPoints();
  //for each round, we have to keep track of the positions of symbols the user wants to change
  // int symbolsToChange[5];
  int positionsToChange[5];
  bool hasBet = false;
  bool hasFinishedRound = false;

  //initialise array with zeros (clear array)
  memset(positionsToChange, 0, sizeof(positionsToChange));

  displayRoundMessage(roundNr, 3000);

  bettingQuestion();

  bool isButtonPressed = false;

  //give user limited time to choose how much to bet
  startTimer();
  unsigned long endTime = startTime + (20 * 1000);
  while (!hasBet && (millis() < endTime)) {
    unsigned long remainingTime = (endTime - millis()) / 1000;
    int tens = remainingTime / 10;
    int unit = remainingTime % 10;
    tm.displayHex(0, tens);
    tm.displayHex(1, unit);

    displayPointsOnLcd();

    uint8_t buttons = tm.readButtons();
    Serial.print("Button state: ");
    Serial.println(buttons);

    if (buttons != 1 && buttons != 128) {
      isButtonPressed = false;
    }

    //user cannot bet less than zero or more than their current points
    if (buttons == 1 && (betPoints - 5) > 0 && !isButtonPressed) {
      betPoints = (betPoints - 5);
      isButtonPressed = true;
      Serial.println("Decreased bet points by 5");
    } else if (buttons == 128 && (betPoints + 5) <= points && !isButtonPressed) {
      betPoints = (betPoints + 5);
      isButtonPressed = true;
      Serial.println("Increased bet points by 5");
    } else {
      Serial.println("Button press not recognized or invalid");
    }

    if (!digitalRead(USER_BTN)) {
      hasBet = true;
    }
  }

  // if the player does not make a choice (and has at least 1 point), then a random amount of points is chosen for the bet
  if (betPoints == 0) {
    if (points > 0) {
      betPoints = random(1, (points + 1));
    }
  }

  displayPoints();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You bet");
  lcd.setCursor(0, 1);
  lcd.print(betPoints);
  lcd.print(" points");

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Now make your");
  lcd.setCursor(0, 1);
  lcd.print("choice");

  delay(2000);
  lcd.clear();

  int randomSequence[5];

  generateRandomSequence(randomSequence, 5, 1, 6);

  // Serial.print("Randomly generated sequence: ");
  for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
    if (i == 0) {
      lcd.setCursor((i), 1);
    } else {
      lcd.setCursor((i + i), 1);
    }
    lcd.printByte(randomSequence[i]);
    Serial.print(randomSequence[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  //now the user has to choose what symbols to swap. When they coose a symbol to swap, they will be marked with an "x" on the upper row of the lcd screen
  startTimer();
  endTime = startTime + (15000);
  while (!hasFinishedRound && (millis() < endTime)) {
    unsigned long remainingTime = (endTime - millis()) / 1000;
    int tens = remainingTime / 10;
    int unit = remainingTime % 10;
    tm.displayHex(0, tens);
    tm.displayHex(1, unit);

    //check if user clicks on any of the respective buttons
    uint8_t buttons = tm.readButtons();
    if (buttons != 0 && buttons != 32 && buttons != 64 && buttons != 128) {
      int btnPressed = findBtnPressed(buttons);
      while (tm.readButtons() != 0) {
        delay(50);
      }

      //if the user already marked the symbol to swap with an "x", unmark it
      if (isArrayEmpty(positionsToChange, 5)) {
        positionsToChange[0] = btnPressed;
        lcd.setCursor((btnPressed - 1) * 2, 0);
        lcd.print("x");
      } else if (elementExists(positionsToChange, 5, btnPressed)) {
        removeElement(positionsToChange, 5, btnPressed);
        lcd.setCursor((btnPressed - 1) * 2, 0);
        lcd.print(" ");
      } else {
        //look for empty space in array and replace it with btnPressed
        addElement(positionsToChange, 5, btnPressed);
        lcd.setCursor((btnPressed - 1) * 2, 0);
        lcd.print("x");
      }
    }

    //user confirms their choices and ends their turn
    if (!digitalRead(USER_BTN)) {
      Serial.println(" ");
      hasFinishedRound = true;
    }
  }

  // replace old sequence with new sequence, where the user has chosen to swap symbols
  // Serial.print("NEW SEQUENCE: ");
  for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
    for (int j = 0; j < (sizeof(positionsToChange) / sizeof(positionsToChange[0])); j++) {
      if (i == (positionsToChange[j] - 1)) {
        int rand = random(1, 6);
        randomSequence[i] = rand;
        break;
      }
    }
    if (i == 0) {
      lcd.setCursor((i), 1);
    } else {
      lcd.setCursor((i + i), 1);
    }
    lcd.printByte(randomSequence[i]);
    Serial.print(randomSequence[i]);
    Serial.print(" ");
  }

  // clear first row of lcd screen (remove "x" marks)
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 0);
    lcd.print(" ");
  }

  //display loading state to the user, to simulate the computer taking a decision
  computerThinking();

  // store how many times each symbol appeared in the user's cards
  int counts[6] = { 0 };

  // for the user: count the occurrences of each number (corresponding to a symbol) in the randomSequence array
  for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
    int num = randomSequence[i];
    counts[num - 1]++;  // Increment the count for the corresponding number
  }

  // determine the final outcome based on how many times each symbol appeared in the user's cards
  String finalOutcome = outcomes[6];  // default outcome is "Nothing"

  // check for each outcome
  for (int i = 0; i < 6; i++) {
    if (counts[i] == 5) {
      finalOutcome = outcomes[0];  // Five of a kind
      break;
    } else if (counts[i] == 4) {
      finalOutcome = outcomes[1];  // Four of a kind
      break;
    } else if (counts[i] == 3) {
      int pairs = 0;
      for (int j = 0; j < 6; j++) {
        if (counts[j] == 2)
          pairs++;
      }
      if (pairs == 1)
        finalOutcome = outcomes[2];  // Full house
      else
        finalOutcome = outcomes[3];  // Three of a kind
      break;
    } else if (counts[i] == 2) {
      int pairs = 0;
      for (int j = 0; j < 6; j++) {
        if (counts[j] == 2)
          pairs++;
      }
      if (pairs == 2)
        finalOutcome = outcomes[4];  // Two pairs
      else
        finalOutcome = outcomes[5];  // One pair
      break;
    }
  }

  // Serial.println(" ");
  // Serial.println("Final Outcome: " + finalOutcome);

  //randomise computer outcome
  int rand = random(0, 29);
  int computerOutcomeIndex = probabilityArray[rand];
  String computerOutcome = outcomes[computerOutcomeIndex];
  // Serial.print("Computer outcome: ");
  // Serial.println(computerOutcome);

  // display the computer outcome
  lcd.setCursor(0, 0);

  if (outcomes[computerOutcomeIndex] == outcomes[0]) {  //five of a kind
    // Serial.println("Five of a kind");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      lcd.printByte(1);
      Serial.print("1");
      Serial.print(" ");
    }
    Serial.println(" ");
  } else if (outcomes[computerOutcomeIndex] == outcomes[1]) {  //four of a kind
    // Serial.println("Four of a kind");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      if (i == 2) {
        lcd.printByte(4);
        Serial.print("4");
      } else {
        lcd.printByte(2);
        Serial.print("2");
      }
      Serial.print(" ");
    }
    Serial.println(" ");
  } else if (outcomes[computerOutcomeIndex] == outcomes[2]) {  //full house
    // Serial.println("Full house");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      if (i == 1 || i == 4) {
        lcd.printByte(5);
        Serial.print("5");
      } else {
        lcd.printByte(3);
        Serial.print("3");
      }
      Serial.print(" ");
    }
    Serial.println(" ");
  } else if (outcomes[computerOutcomeIndex] == outcomes[3]) {  //three of a kind
    // Serial.println("Three of a kind");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      if (i == 1) {
        lcd.printByte(5);
        Serial.print("5");
      } else if (i == 4) {
        lcd.printByte(4);
        Serial.print("4");
      } else {
        lcd.printByte(3);
        Serial.print("3");
      }
      Serial.print(" ");
    }
    Serial.println(" ");
  } else if (outcomes[computerOutcomeIndex] == outcomes[4]) {  //two pairs
    // Serial.println("Two pairs");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      if (i == 1 || i == 4) {
        lcd.printByte(1);
        Serial.print("1");
      } else if (i == 3) {
        lcd.printByte(4);
        Serial.print("4");
      } else {
        lcd.printByte(2);
        Serial.print("2");
      }
      Serial.print(" ");
    }
    Serial.println(" ");
  } else if (outcomes[computerOutcomeIndex] == outcomes[5]) {  //one pair
    // Serial.println("One pair");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      if (i == 0 || i == 3) {
        lcd.printByte(4);
        Serial.print("4");
      } else if (i == 1) {
        lcd.printByte(1);
        Serial.print("1");
      } else if (i == 2) {
        lcd.printByte(2);
        Serial.print("2");
      } else if (i == 4) {
        lcd.printByte(3);
        Serial.print("3");
      }
      Serial.print(" ");
    }
    Serial.println(" ");
  } else {  //nothing
    // Serial.println("Nothing");
    for (int i = 0; i < (sizeof(randomSequence) / sizeof(randomSequence[0])); i++) {
      if (i == 0) {
        lcd.setCursor((i), 0);
      } else {
        lcd.setCursor((i + i), 0);
      }
      lcd.printByte((i + 1));
      Serial.print((i + 1));

      Serial.print(" ");
    }
  }

  // compare outcomes and print result
  int playerScore = outcomeScores[findIndex(outcomes, finalOutcome)];
  int computerScore = outcomeScores[findIndex(outcomes, computerOutcome)];

  if (playerScore > computerScore) {
    // Serial.println("Player wins the round!");
    delay(3000);
    points = points + betPoints * playerScore;
    lcd.setCursor(12, 0);
    lcd.print("WON!");
    buzzer.playWinSound();
  } else if (playerScore < computerScore) {
    // Serial.println("Computer wins the round!");
    delay(3000);
    points = points - betPoints;
    lcd.setCursor(11, 0);
    lcd.print("LOST!");
    buzzer.playLoseSound();
  } else {
    // Serial.println("It's a tie!");
    lcd.setCursor(12, 0);
    lcd.print("TIE!");
  }
  // Serial.print("POINTS: ");
  // Serial.println(points);
  // displayPoints();
  // Serial.println("-------------------------------------------------------------------");

  //reset variables
  betPoints = 0;
}

void displayPointsOnLcd() {
  lcd.setCursor(12, 1);
  if (betPoints < 100) {
    lcd.print("0");  // leading zero
  }
  if (betPoints < 10) {
    lcd.print("0");
  }
  lcd.print(betPoints);
}

void bettingQuestion() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("How much do you");
  lcd.setCursor(0, 1);
  lcd.print("wanna bet?");
}

// Function to find the index of an element in an array of strings
int findIndex(String array[], String element) {
  for (int i = 0; i < 7; i++) {
    if (array[i] == element) {
      return i;
    }
  }
  return -1;  // Element not found
}

void addElement(int array[], int size, int valueToAdd) {
  for (int i = 0; i < size; i++) {
    if (array[i] == 0) {
      array[i] = valueToAdd;
      break;
    }
  }
}

void removeElement(int array[], int size, int valueToRemove) {
  for (int i = 0; i < size; i++) {
    if (array[i] == valueToRemove) {
      array[i] = 0;
    }
  }
}

bool elementExists(int array[], int size, int element) {
  for (int i = 0; i < size; i++) {
    if (array[i] == element) {
      return true;  // Element found in the array
    }
  }
  return false;  // Element not found in the array
}

bool isArrayEmpty(int arr[], int size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] != 0) {
      return false;
    }
  }
  return true;
}

// Display dots for 5 seconds (one more dot each second until 5)
void computerThinking() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.print(" ");
  }

  for (int i = 0; i < 5; i++) {
    lcd.setCursor(i, 0);
    lcd.print(".");
    delay(1000);
  }

  for (int i = 0; i < 5; i++) {
    lcd.setCursor(i, 0);
    lcd.print(" ");
  }
}