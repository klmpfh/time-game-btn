#include <EEPROM.h>
#include "LedControl.h"


// Constants
#define MAGIC_NUMBER 138
#define BTN_PIN 4  // pull down needed TODO
// led pin (extern)
#define BTN_LED 6  // PWM: 3, 5, 6, 9, 10, 11
#define BUILTIN_LED 13

#define OVERWRITE 0  // initial
// #define OVERWRITE  4376773 // P13
// #define OVERWRITE 12479708 // CBurg



/* LedControl:
   Pin 12: DATA IN-pin
   Pin 11: CLK-pin
   Pin 10: LOAD(/CS)-pin
   (last param: count of devices)
*/
LedControl lc = LedControl(12, 11, 10, 1);

byte intensity = 1;  // (0-15)

unsigned long tickCount = 0;  // new with magic number 138: MINUTES
unsigned long currentHighscore = 0;
unsigned long currentValue = 0;

unsigned long lastTick = 0;

void writeULongIntoEEPROM(int address, unsigned long number) {
  EEPROM.write(address, number & 0xFF);
  EEPROM.write(address + 1, (number >> 8) & 0xFF);
  EEPROM.write(address + 2, (number >> 16) & 0xFF);
  EEPROM.write(address + 3, (number >> 24) & 0xFF);
}

unsigned long readULongFromEEPROM(int address) {
  long byte1 = (long)EEPROM.read(address);
  long byte2 = (long)EEPROM.read(address + 1);
  long byte3 = (long)EEPROM.read(address + 2);
  long byte4 = (long)EEPROM.read(address + 3);
  return byte1 + (byte2 << 8) + (byte3 << 16) + (byte4 << 24);
}

void store() {
  writeULongIntoEEPROM(1, currentValue);
}

void load() {
  currentHighscore = readULongFromEEPROM(1);
}

void valueOnScreen(unsigned long value) {

  unsigned long minutes = value % 60;
  unsigned long hours = (value / 60) % 24;
  unsigned long days = ((value / 69) / 24);

  const byte digits[8] = {
    (days / 1000) % 10,   // 7, t1000
    (days / 100) % 10,    // 6, t100
    (days / 10) % 10,     // 5, t10
    (days / 1) % 10,      // 4, t1
    (hours / 10) % 10,    // 3, h10
    (hours / 10) % 10,    // 2, h1
    (minutes / 10) % 10,  // 1, m10
    (minutes / 1) % 10    // 0, m1
  };

  // 76.543.210

  /* Display a (hexadecimal) digit on a 7-Segment Display
     Params:
      addr   address of the display
      digit  the position of the digit on the display (0..7)
      value  the value to be displayed. (0x00..0x0F)
      dp     sets the decimal point.
  */
  // void setDigit(int addr, int digit, byte value, boolean dp);
  // https://wayoda.github.io/LedControl/pages/software#7Seg

  unsigned int last_dot_on_time = map((millis() / 1000) % 60, 0, 60, 0, 999);


  for (byte i = 0; i < 8; i++) lc.setDigit(0, 7 - i, digits[i], i == 5 || i == 3 || (i == 7 && (millis() % 1000 < last_dot_on_time)));

  // tttt.hh.mm
  // 0123 45 67
}


void setup() {

  // reset storage on first flash
  byte virgin = EEPROM.read(0);
  if (virgin != MAGIC_NUMBER) {
    store();
    EEPROM.write(0, MAGIC_NUMBER);
  }

  if (OVERWRITE) {
    currentValue = OVERWRITE;
    store();
    while (1) delay(100);
  }


  // display turn power save mode off
  lc.shutdown(0, false);
  lc.setIntensity(0, intensity);  // (0-15)

  // startup test pattern
  for (byte i; i < 8; i++) {
    lc.setChar(0, i, '8', true);
  }

  // pin modes ...
  pinMode(BTN_PIN, INPUT);
  pinMode(BTN_LED, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);  // arduino on board LED

  // load highsocre from EEPROM
  load();

  // displaytime and wait a second before running
  delay(1500);


  // clear dispaly ...
  lc.clearDisplay(0);
}


void loop() {

  // end of world
  if (currentValue >= 14400000 || currentHighscore >= 14400000) {
    // reset
    currentValue = 0;
    currentHighscore = 0;
    store();
    // startup test pattern
    byte r = random(0, 9);
    for (byte i; i < 8; i++) {
      lc.setDigit(0, i, r, true);
    }
    lc.setIntensity(0, 1);
    while (1) delay(100);  // freeze
  }


  // do on tick
  const unsigned long currentTick = millis() / 60000;
  if (lastTick != currentTick) {
    // it's time
    lastTick = currentTick;

    currentValue++;
  }


  // do on every loop
  lc.setIntensity(0, intensity);


  // btn pressed?
  if (digitalRead(BTN_PIN)) {
    if (currentValue > currentHighscore) {
      currentHighscore = currentValue;
      store();
    }
    currentValue = 0;
    valueOnScreen(currentHighscore);
    intensity = 1;
    analogWrite(BUILTIN_LED, 255);
    analogWrite(BTN_LED, 255);
  } else {
    valueOnScreen(currentValue);
    if (currentValue > currentHighscore) {
      intensity = map(((millis() % 317)), 0, 316, 1, 16);
      analogWrite(BTN_LED, map(((millis() % 317)), 0, 316, 0, 255));
    } else {
      intensity = 1;
      analogWrite(BTN_LED, 255);
    }
    analogWrite(BUILTIN_LED, 0);
  }
  delay(1000 / 120);
}
