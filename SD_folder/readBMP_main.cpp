#include <iostream>
#include <string>
using namespace std;

#include <fstream>
using std::cout; using std::ofstream;
using std::endl; using std::string;
using std::fstream;

#include "Arduino.h"
#include "esp_system.h"

//#include "FS.h"
#include "SD.h"
//#include "SPI.h"
#include "main_SD.h"
#include "ESP32Time.h"
#include "time.h"
#include <sys/time.h>

#define SCK_SD   5
#define MOSI_SD  18
#define MISO_SD  19
#define CS_SD    17

SPIClass spi_SD = SPIClass(VSPI);

void power_button() {

  if (digitalRead(powr_but)) {
    while (digitalRead(powr_but));
    esp_deep_sleep_start();
  }

}

void setup_SD(){
  //Set up SPI for SD
  spi_SD.begin(SCK_SD, MISO_SD, MOSI_SD, CS_SD);

  // Set up SD
  if(!SD.begin(CS_SD, spi_SD, 80000000)){
    // failed
    return;
  }

  // check for SD card
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    // no card
    return;
  }
}

// setup code
#define BUFFPIXEL 32
uint8_t  sdbuffer[3*BUFFPIXEL];

void setup()
{
  // initialize LED digital pin as an output.
  //pinMode(led21, OUTPUT);
  //pinMode(led23, OUTPUT);
  //pinMode(mode_but, INPUT);
  //pinMode(powr_but, INPUT);
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, HIGH);

  // SD setup
  setup_SD();
  //digitalWrite(23, HIGH);
  bmpDraw(SD, "/test_bmp.bmp", sdbuffer);
}

// Main Code
void loop() {

  // Check power button
  //if (sleep_last) {
  //  sleep_last = digitalRead(powr_but);
  //}
  //else {
  //  power_button();
  //}

}
