#include "WSerial.h"
#include "wiring_time.h"
#include "challenge3.h"

void startTimer();
void displayPoints();
void startChallengeMessage(int challengeNr);
void doneChallengeMessage(int challengeNr);
int findBtnPressed(int binaryNr);

unsigned long previousMillis = 0;
const long interval = 200;  // interval between each LED lighting up
bool hasPlayedLastLight = false;
//make sure next random led is not current random led
int previousRandomIndex = -1;
int previousRandomLED = -1;
int whackcounter = 0; //count how many leds light up

void challenge3() {

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  startChallengeMessage(3);

  //first round: whack a mole
  displayRound(1, 3000);
  while (whackcounter <= 10) {
    whackamoleSequence();
    whackcounter++;
  }

  //second round: catch the lightning
  displayRound(2, 3000);
  while (!hasPlayedLastLight) {
    lightningSequence();
  }

  doneChallengeMessage(3);
}

void whackamoleSequence() {
  int randomLED;

  // generate a new random number until it's different from the previous one
  do {
    randomLED = random(0, 8);
  } while (randomLED == previousRandomLED);

  // Serial.print(randomLED);
  tm.setLED(randomLED, 1);
  unsigned long lightUpTime = millis();
  bool hasGuessed = false;

  startTimer();
  unsigned long endTime = startTime + (5 * 1000);

  while (!hasGuessed && (millis() < endTime)) {
    unsigned long remainingTime = (endTime - millis()) / 1000;
    int tens = remainingTime / 10; 
    int ones = remainingTime % 10;  
    tm.displayHex(0, tens);
    tm.displayHex(1, ones);

    // Check if user presses button
    uint8_t buttons = tm.readButtons();
    if (buttons != 0) {
      int btnPressed = findBtnPressed(buttons);
      if (btnPressed == randomLED + 1) {                      // checking if the button pressed corresponds to the LED
        unsigned long reactionTime = millis() - lightUpTime; 
        Serial.print("Pressed right button. Reaction time: ");
        Serial.print(reactionTime);
        Serial.println(" ms");
        if (reactionTime <= 500) {
          points = points + 2;
        } else if (reactionTime > 500 && reactionTime < 1000) {
          points = points + 1;
        }
        displayPoints();
        hasGuessed = true;
        tm.setLED(randomLED, 0);        // turn off the LED
        previousRandomLED = randomLED;  // update the previous random number
      }
    }
  }
}

void lightningSequence() {
  Serial.println("in lightning loop");
  static int cycleCount = 0;                // track the number of complete cycles
  static bool randomLightSelected = false;  // flag to track if random light has been selected
  static int randomIndex = 0;
  bool hasPressed = false;

  // check if it's time to light up the next LED
  if (millis() - previousMillis >= interval) {
    Serial.println("time to light it up");
    // reset the interval timer
    previousMillis = millis();

    // light up LEDs from left to right
    for (int i = 0; i < 8; i++) {
      tm.setLED(i, 1);
      delay(20);  // Adjust speed as desired
      tm.setLED(i, 0);
      delay(20);  // Adjust speed as desired
    }

    // light up LEDs from right to left
    for (int i = 7; i >= 0; i--) {
      tm.setLED(i, 1);
      delay(20);  // Adjust speed as desired
      tm.setLED(i, 0);
      delay(20);  // Adjust speed as desired
    }

    // increment cycle count after lighting all leds up from right to left and viceversa
    cycleCount++;

    // if completed 8 cycles, select a random index and stop
    if (cycleCount >= 4 && !randomLightSelected) {
      randomIndex = random(0, 8);  
      randomLightSelected = true;  
    }

    // after random light is selected, light up that index
    if (randomLightSelected) {
      Serial.println("random light was selected");
      for (int i = 0; i < randomIndex; i++) {
        Serial.println(i);
        if (i == (randomIndex - 1)) {
          tm.setLED(i, 1);
          unsigned long lightUpTime = millis();
          startTimer();
          unsigned long endTime = startTime + (5 * 1000);
          while (!hasPressed && (millis() < endTime)) {
            unsigned long remainingTime = (endTime - millis()) / 1000;
            int tens = remainingTime / 10; 
            int ones = remainingTime % 10;  
            tm.displayHex(0, tens);
            tm.displayHex(1, ones);
            
            uint8_t buttons = tm.readButtons();
            if (buttons != 0) {
              int btnPressed = findBtnPressed(buttons);
              if (btnPressed == randomIndex) {
                Serial.println("has pressed the right btn");          
                unsigned long reactionTime = millis() - lightUpTime; 
                Serial.print("LIGHTNING Reaction time: ");
                Serial.print(reactionTime);
                Serial.println(" ms");
                hasPressed = true;
                tm.setLED(i, 0);  // Turn off the LED
                if (reactionTime <= 500) {
                  points = points + 4;
                } else {
                  points = points + 2;
                }
                displayPoints();
              } else {
                Serial.println("has not pressed the right btn");
              }
            }
          }
          hasPlayedLastLight = true;
        } else {
          tm.setLED(i, 1);
          delay(20);  
          tm.setLED(i, 0);
          delay(20);  
        }
      }
    }
  }
}

void displayRound(int roundNr, int delayVal) {
  lcd.clear();
  if (roundNr == 1) {
    lcd.setCursor(4, 0);
    lcd.print("WHACK A");
    lcd.setCursor(6, 1);
    lcd.print("MOLE");
  } else if (roundNr == 2) {
    lcd.setCursor(3, 0);
    lcd.print("CATCH THE");
    lcd.setCursor(3, 1);
    lcd.print("LIGHTNING");
  } else if (roundNr == 3) {
    lcd.setCursor(5, 0);
    lcd.print("ODD ONE");
    lcd.setCursor(6, 1);
    lcd.print("OUT");
  } else {
    lcd.setCursor(6, 0);
    lcd.print("error");
  }
  delay(delayVal);
  lcd.clear();
}