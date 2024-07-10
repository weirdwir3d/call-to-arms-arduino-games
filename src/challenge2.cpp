#include "challenge2.h"

void startTimer();
void displayPoints();
void doneChallengeMessage(int challengeNr);
void startChallengeMessage(int challengeNr);

int range1 = 255;
int range2 = 510;
int range3 = 765;
int range4 = 1023;
bool isButtonPressed = false;
int answers[] = { 1, 2, 3, 4 };
int inputAnswer = 0;
int userInput = 0;

void challenge2() {
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  startChallengeMessage(2);

  //----------FIRST QUESTION----------
  //what is the primary ability score used by wizards to determine the potency of their spells?
  //1) strength, 2) dexterity, 3) intelligence, 4) wisdom
  playRound(1, 10, 1000);

  //----------SECOND QUESTION----------
  // //Which class in Dungeons & Dragons is known for its ability to transform into animals and harness nature-based magic?
  // //1) Barbarian, 2) Rogue, 3) Druid, 4) Paladin
  playRound(2, 11, 1000);

  //----------THIRD QUESTION----------
  // //Which of the following is NOT a type of damage commonly used in combat encounters in D&D?
  // //1) Slashing, 2) Fire, 3) Radiant, 4) Sonic
  playRound(3, 12, 1000);

  //----------FOURTH QUESTION----------
  // //Which creature type is vulnerable to radiant damage in Dungeons & Dragons?
  // //1) Undead, 2) Constructs, 3) Dragons, 4) Elementals
  playRound(4, 13, 1000);

  doneChallengeMessage(2);
}

void playRound(int questionNr, int time, int delayVal) {
  displayQuestion(questionNr, delayVal);
  startTimer();
  unsigned long endTime = startTime + (time * 1000);
  while (!isButtonPressed && (millis() < endTime)) {
    unsigned long remainingTime = (endTime - millis()) / 1000;
    int tens = remainingTime / 10;
    int ones = remainingTime % 10;
    tm.displayHex(0, tens);
    tm.displayHex(1, ones);

    int value = analogRead(POTENTIOMETER_PIN);
    showOptions(value);
  }
  displayFeedback(questionNr, delayVal);

  isButtonPressed = false;
}

void displayFeedback(int questionNr, int delayVal) {
  delay(delayVal);
  int index = questionNr - 1;
  int rightAnswer = answers[index];
  Serial.print("user input: ");
  Serial.print(userInput);
  Serial.println("right answer: ");
  Serial.print(rightAnswer);
  if (userInput == rightAnswer) {
    feedbackCorrectAnswer(questionNr);
  } else {
    feedbackWrongAnswer();
  }
  delay(delayVal);
}

void showOptions(int value) {
  lcd.setCursor(0, 0);
  if (value >= 0 && value < range1) {
    inputAnswer = 1;
    blinkNumber("1", 0, 0);
    // Serial.print("ONE");
  } else {
    lcd.print("1");
  }

  lcd.setCursor(9, 0);
  if (value > range1 && value < range2) {
    inputAnswer = 2;
    blinkNumber("2", 9, 0);
    // Serial.print("TWO");
  } else {
    lcd.print("2");
  }

  lcd.setCursor(0, 1);
  if (value > range2 && value < range3) {
    inputAnswer = 3;
    blinkNumber("3", 0, 1);
    // Serial.print("THREE");
  } else {
    lcd.print("3");
  }

  lcd.setCursor(9, 1);
  if (value > range3 && value <= range4) {
    inputAnswer = 4;
    blinkNumber("4", 9, 1);
    // Serial.print("FOUR");
  } else {
    lcd.print("4");
  }

  if (!digitalRead(USER_BTN)) {
    Serial.print("PRESSED");
    isButtonPressed = true;
    userInput = inputAnswer;
  }
}

void displayQuestion(int questionNr, int delayVal) {
  lcd.clear();
  if (questionNr == 1) {
    lcd.setCursor(6, 0);
    lcd.print("FIRST");
    lcd.setCursor(4, 1);
    lcd.print("QUESTION");
  } else if (questionNr == 2) {
    lcd.setCursor(6, 0);
    lcd.print("SECOND");
    lcd.setCursor(6, 1);
    lcd.print("QUESTION");
  } else if (questionNr == 3) {
    lcd.setCursor(5, 0);
    lcd.print("THIRD");
    lcd.setCursor(6, 1);
    lcd.print("QUESTION");
  } else if (questionNr == 4) {
    lcd.setCursor(5, 0);
    lcd.print("FOURTH");
    lcd.setCursor(6, 1);
    lcd.print("QUESTION");
  } else {
    lcd.setCursor(6, 0);
    lcd.print("error");
  }
  delay(delayVal);
  lcd.clear();
}

//make numbers blink on screen
void blinkNumber(String number, int col, int row) {
  static bool showNumber = true;  
  showNumber = !showNumber;       
  lcd.setCursor(col, row);
  if (showNumber) {
    lcd.print(number);
  } else {
    lcd.print(" ");  
  }
  delay(500);
}

void feedbackCorrectAnswer(int questionNr) {
  if (questionNr == 1) {
      points = points + 5;
    } else if (questionNr == 2) {
      points = points + 10;
    } else if (questionNr == 3) {
      points = points + 15;
    } else if (questionNr == 4) {
      points = points + 20;
    }
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("CORRECT");
  buzzer.playWinSound();
  displayPoints();
  delay(2000);
  lcd.clear();
}

void feedbackWrongAnswer() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("WRONG");
  buzzer.playLoseSound();
  delay(2000);
  lcd.clear();
}

void promptForAnswer() {
  lcd.setCursor(3, 0);
  lcd.print("SELECT THE");
  lcd.setCursor(2, 1);
  lcd.print("RIGHT ANSWER");
  delay(1000);
  lcd.clear();
}