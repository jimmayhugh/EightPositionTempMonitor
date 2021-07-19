/***************************************************
  eightPositionTempMonitor.ino 
  Modified by Jim Mayhugh
  07/16/2021
  Now exercises all eight available I2C 7 Segment Backpacks
  Added eight DS18B20 digital Temperature Probes, and output the results
  to the backpacks
  
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "OneWire.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// function prototypes
void findChips(void);
void startTemp(void);
void readTemp(void);
// end function prototypes

// Debug Info
uint32_t findChipsDebug = 0x0001;
uint32_t setDebug       = 0x0001;
// end Debug Info

// Seven Segment I2C display info
Adafruit_7segment LED0 = Adafruit_7segment();
Adafruit_7segment LED1 = Adafruit_7segment();
Adafruit_7segment LED2 = Adafruit_7segment();
Adafruit_7segment LED3 = Adafruit_7segment();
Adafruit_7segment LED4 = Adafruit_7segment();
Adafruit_7segment LED5 = Adafruit_7segment();
Adafruit_7segment LED6 = Adafruit_7segment();
Adafruit_7segment LED7 = Adafruit_7segment();

Adafruit_7segment *led[] = { &LED0, &LED1, &LED2, &LED3, &LED4, &LED5, &LED6, &LED7 };

uint8_t  maxLEDS = 8;
uint8_t  port    = 0x70;
uint8_t x = 0;
// end of Seven Segment display info

// DS18B20 info

uint8_t chipCnt;

const uint8_t maxDS           =   8; // max number of DS18B20s 
const uint8_t chipAddrSize    =   8; // 64bit OneWire Address
const uint8_t tempDataSize    =   9; // temp data


uint8_t dsPin0 = 14;
uint8_t dsPin1 = 15;
uint8_t dsPin2 = 16;
uint8_t dsPin3 = 17;
uint8_t dsPin4 = 20;
uint8_t dsPin5 = 21;
uint8_t dsPin6 = 22;
uint8_t dsPin7 = 23;

OneWire ds0(dsPin0);
OneWire ds1(dsPin1);
OneWire ds2(dsPin2);
OneWire ds3(dsPin3);
OneWire ds4(dsPin4);
OneWire ds5(dsPin5);
OneWire ds6(dsPin6);
OneWire ds7(dsPin7);

OneWire *ds[] = { &ds0, &ds1, &ds2, &ds3, &ds4, &ds5, &ds6, &ds7 };

typedef struct
{
  uint8_t     tempAddress[chipAddrSize];
  uint8_t     tempData[tempDataSize];
  float       tempC;
}tempStruct;

const tempStruct tempClear = { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0};

tempStruct ds18b20[maxDS] = {
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0},
                              {{0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0.0}
                            };
                              

// end of DS18B20 info

uint32_t baudrate = 115200;

void setup()
{
#ifndef __AVR_ATtiny85__
  Serial.begin(baudrate);
  delay(1000);
  Serial.println("7 Segment Backpack Test");
#endif

  for(x=0;x<maxLEDS;x++)
  {
    led[x]->begin(port+x);
  }

}

void loop()
{
  findChips();
  startTemp();
  delay(1000);
  readTemp();
/*
  for(x=0;x<maxLEDS;x++)
  {
    // try to print a number thats too long
    led[x]->print(10000, DEC);
    led[x]->writeDisplay();
    delay(50);
  }

  for(x=0;x<maxLEDS;x++)
  {
  // print a hex number
    led[x]->print(0xBEEF, HEX);
    led[x]->writeDisplay();
    delay(50);
  }
*/
  for(x=0;x<maxLEDS;x++)
  {
    // print a floating point 
    led[x]->print(ds18b20[x].tempC,1);
    led[x]->writeDisplay();
    delay(50);
  }

/*  
  // print with print/println
  for (uint16_t counter = 0; counter < 99; counter++)
  {
    for(x=0;x<maxLEDS;x++)
    {
      led[x]->println(counter);
      led[x]->writeDisplay();
      delay(10);
    }
  }

/*
  // method #2 - draw each digit
  uint16_t blinkcounter = 0;
  boolean drawDots = false;
  for (uint16_t counter = 0; counter < 9999; counter ++)
  {
    for(x=0;x<maxLEDS;x++)
    {
      led[x]->writeDigitNum(0, (counter / 1000), drawDots);
      led[x]->writeDigitNum(1, (counter / 100) % 10, drawDots);
      led[x]->drawColon(drawDots);
      led[x]->writeDigitNum(3, (counter / 10) % 10, drawDots);
      led[x]->writeDigitNum(4, counter % 10, drawDots);
     
      blinkcounter+=50;
      if (blinkcounter < 500)
      {
        drawDots = false;
      } else if (blinkcounter < 1000) {
        drawDots = true;
      } else {
        blinkcounter = 0;
      }
      led[x]->writeDisplay();
      delay(10);
    }
  }
*/
}
