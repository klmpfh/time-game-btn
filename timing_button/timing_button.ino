
#include <EEPROM.h>
#include "LedControl.h"

/* Create a new LedControl variable.
 * We use pins 12,11 and 10 on the Arduino for the SPI interface
 * Pin 12 is connected to the DATA IN-pin of the first MAX7221
 * Pin 11 is connected to the CLK-pin of the first MAX7221
 * Pin 10 is connected to the LOAD(/CS)-pin of the first MAX7221
 * There will only be a single MAX7221 attached to the arduino 
 */  
LedControl lc = LedControl(12,11,10,1);

int current_days = 0; // 2 byte
byte current_hours = 0; // 1 byte
byte current_minutes = 0; // 1 byte
byte current_secounds = 0; // 1 byte
byte current_deci_secounds = 0; // 1 byte

unsigned int highscore_days = 0; // 2 byte
byte highscore_hours = 0; // 1 byte
byte highscore_minutes = 0; // 1 byte
byte highscore_secounds = 0; // 1 byte
byte highscore_deci_secounds = 0; // 1 byte

#define btn_pin 4  // pull down needed TODO
#define btn_led 6 // PWM: 3, 5, 6, 9, 10, 11

// https://polluxlabs.net/arduino-tutorials/eine-7-segment-anzeige-am-arduino-anschliessen-und-verwenden/

void setup() {

  Serial.begin(9600);

  // reset storeage on first flash
  // reset_highscore(); // remove this on secound flash!
  
  lc.shutdown(0,false);
  lc.setIntensity(0,1);

  lc.setChar( 0, 0, '8', true);
  lc.setChar( 0, 1, '8', true);
  lc.setChar( 0, 2, '8', true);
  lc.setChar( 0, 3, '8', true);
  lc.setChar( 0, 4, '8', true);
  lc.setChar( 0, 5, '8', true);
  lc.setChar( 0, 6, '8', true);
  lc.setChar( 0, 7, '8', true);
  
  pinMode(btn_pin, INPUT);
  pinMode(btn_led, OUTPUT);
  pinMode(13, OUTPUT);
  
  load_higscore();

  delay(1500);

  lc.clearDisplay(0);
}

void displayValues(bool current){
  unsigned int display_day = current ? current_days : highscore_days;
  byte display_hours = current ? current_hours : highscore_hours;
  byte display_minutes = current ? current_minutes : highscore_minutes;
  byte display_secounds = current ? current_secounds : highscore_secounds;
  byte display_deci_secounds = current ? current_deci_secounds : highscore_deci_secounds;

  const bool off_dott = overHighscore() ? ((millis() / 123) % 8 < 2) : false;
  
  /* Display a (hexadecimal) digit on a 7-Segment Display
   * Params:
   *  addr   address of the display
   *  digit  the position of the digit on the display (0..7)
   *  value  the value to be displayed. (0x00..0x0F)
   *  dp     sets the decimal point.  
   */  
  // void setDigit(int addr, int digit, byte value, boolean dp);
  // https://wayoda.github.io/LedControl/pages/software#7Seg

  // 543210
  //       9876543210
  //        d  h
  // ttttttt hh mmssx


  if(display_day <= 0){
    // HHhmm.ss.x
    lc.setDigit( 0, 0, (byte) (display_deci_secounds % 10), off_dott);
    lc.setDigit( 0, 1, (byte) (display_secounds % 10), true); // s 1
    lc.setDigit( 0, 2, (byte) ((display_secounds / 10) % 10), off_dott); // s 10
    lc.setDigit( 0, 3, (byte) (display_minutes % 10), true); // m 1
    lc.setDigit( 0, 4, (byte) ((display_minutes / 10) % 10), off_dott); // m 10
    lc.setChar(  0, 5, 'h', off_dott);
    lc.setDigit( 0, 6, (byte) (display_hours % 10) , off_dott); // h 1
    lc.setDigit( 0, 7, (byte) ((display_hours / 10) % 10) , off_dott); // h 10
    return;
  }

  if(display_day < 100){
    // TTdHHhmm
    lc.setDigit( 0, 0, (byte) (display_minutes % 10), off_dott); // m 1
    lc.setDigit( 0, 1, (byte) ((display_minutes / 10) % 10), off_dott); // m 10
    lc.setChar(  0, 2, 'h', off_dott);
    lc.setDigit( 0, 3, (byte) (display_hours % 10), off_dott); // h 1
    lc.setDigit( 0, 4, (byte) ((display_hours / 10) % 10), off_dott); // h 10
    lc.setChar(  0, 5, 'd', off_dott);
    lc.setDigit( 0, 6, (byte) (display_day % 10), off_dott); // t 1
    lc.setDigit( 0, 7, (byte) ((display_day / 10) % 10), off_dott); // t 10
    return;
  }

  if(display_day < 10000000){
    // TTTTTTTd
    lc.setChar(  0, 0, 'd', off_dott);
    lc.setDigit( 0, 1, (byte) (display_day % 10), off_dott); // t 1
    lc.setDigit( 0, 2, (byte) ((display_day / 10) % 10), off_dott); // t 10
    lc.setDigit( 0, 3, (byte) ((display_day / 100) % 10), off_dott); // t 100
    lc.setDigit( 0, 4, (byte) ((display_day / 1000) % 10), off_dott); // t 1000
    lc.setDigit( 0, 5, (byte) ((display_day / 10000) % 10), off_dott); // t 10000
    lc.setDigit( 0, 6, (byte) ((display_day / 100000) % 10), off_dott); // t 100000
    lc.setDigit( 0, 7, (byte) ((display_day / 1000000) % 10), off_dott); // t 1000000
    return;
  }

  
  // TTTTTTTT
  lc.setDigit( 0, 0, (byte) (display_day % 10), off_dott); // t 1
  lc.setDigit( 0, 1, (byte) ((display_day / 10) % 10), off_dott); // t 10
  lc.setDigit( 0, 2, (byte) ((display_day / 100) % 10), off_dott); // t 100
  lc.setDigit( 0, 3, (byte) ((display_day / 1000) % 10), off_dott); // t 1000
  lc.setDigit( 0, 4, (byte) ((display_day / 10000) % 10), off_dott); // t 10000
  lc.setDigit( 0, 5, (byte) ((display_day / 100000) % 10), off_dott); // t 100000
  lc.setDigit( 0, 6, (byte) ((display_day / 1000000) % 10), off_dott); // t 1000000
  lc.setDigit( 0, 7, (byte) ((display_day / 10000000) % 10), off_dott); // t 10000000
  return;

}

bool overHighscore(){

  if(current_days > highscore_days) return true;
  
  if(current_days == highscore_days){
    if(current_hours > highscore_hours) return true;
    if(current_hours == highscore_hours){
      if(current_minutes > highscore_minutes) return true;
      if(current_minutes == highscore_minutes){
        if(current_secounds > highscore_secounds) return true;
        if(current_secounds == highscore_secounds){
          if(current_deci_secounds > highscore_deci_secounds) return true;
        }
      }
    }
  }
  
  return false;
}

void printSerial(){

  Serial.print(current_days);
  Serial.print(".");
  Serial.print(current_hours);
  Serial.print(".");
  Serial.print(current_minutes);
  Serial.print(".");
  Serial.print(current_secounds);
  Serial.print(".");
  Serial.print(current_deci_secounds);
  Serial.print("  -  ");
  Serial.print(highscore_days);
  Serial.print(".");
  Serial.print(highscore_hours);
  Serial.print(".");
  Serial.print(highscore_minutes);
  Serial.print(".");
  Serial.print(highscore_secounds);
  Serial.print(".");
  Serial.print(highscore_deci_secounds);
  Serial.print("  -  ");
  Serial.print(overHighscore());
  Serial.println("");
}

void loop() {

  printSerial();

  if(digitalRead(btn_pin)){
    // btn pressed

    if(overHighscore()){
      save_highscore();
    }
    
    // display highscore
    displayValues(false);


    // btn led on
    analogWrite(btn_led, 255);
    analogWrite(13, 255);

    while(digitalRead(btn_pin)) delay(10);

    current_days = 0;
    current_hours = 0;
    current_minutes = 0;
    current_secounds = 0;
    current_deci_secounds = millis()/100%10;
    displayValues(true);
    
  }else{
    // run time
    // is deci secound changed?
    if(millis()/100%10 != current_deci_secounds){
      // update time and display
  
      current_deci_secounds++;
  
      if(current_deci_secounds >= 10){
        current_deci_secounds = 0;
        current_secounds++;
      }
      
      if(current_secounds >= 60){
        current_secounds = 0;
        current_minutes++;
      }
      
      if(current_minutes >= 60){
        current_minutes = 0;
        current_hours++;
      }
      
      if(current_hours >= 24){
        current_hours = 0;
        current_days++;
      }
  
      // display value on display
      displayValues(true);
      
    }
    

    // btn LED
    if(overHighscore()){
      analogWrite(btn_led, map(millis()%320,0,320,0,255));
      analogWrite(13, map(millis()%320,0,320,0,255));
    }else{
      
      analogWrite(btn_led, map(millis()%4321,0,4320,0,255));
      analogWrite(13, map(millis()%4321,0,4320,0,255));
    }
    
  }
}


void save_highscore(){

  // days
  byte byte1 = current_days >> 8;
  byte byte2 = current_days & 0xFF;
  EEPROM.update(0, byte1);
  EEPROM.update(1, byte2);
  // hours
  EEPROM.update(2, current_hours);
  // minutes
  EEPROM.update(3, current_minutes);
  // secounds
  EEPROM.update(4, current_secounds);
  // secounds
  EEPROM.update(5, current_deci_secounds);

  highscore_days = current_days;
  highscore_hours = current_hours;
  highscore_minutes = current_minutes;
  highscore_secounds = current_secounds;
  highscore_deci_secounds = current_deci_secounds;

  current_days = 0;
  current_hours = 0;
  current_minutes = 0;
  current_secounds = 0;
  current_deci_secounds = 0;
  
}

void reset_highscore(){

  // days
  EEPROM.update(0, 0);
  EEPROM.update(1, 0);
  // hours
  EEPROM.update(2, 0);
  // minutes
  EEPROM.update(3, 0);
  // secounds
  EEPROM.update(4, 0);
  // secounds
  EEPROM.update(5, 0);

  highscore_days = 0;
  highscore_hours = 0;
  highscore_minutes = 0;
  highscore_secounds = 0;
  highscore_deci_secounds = 0;

  current_days = 0;
  current_hours = 0;
  current_minutes = 0;
  current_secounds = 0;
  current_deci_secounds = 0;
  
}

void load_higscore(){
  // highscore_days
  EEPROM.get(0, highscore_days);
  // highscore_hours
  EEPROM.get(2, highscore_hours);
  // highscore_minutes
  EEPROM.get(3, highscore_minutes);
  // highscore_secounds
  EEPROM.get(4, highscore_secounds);
  // 
  EEPROM.get(5, highscore_deci_secounds);
}