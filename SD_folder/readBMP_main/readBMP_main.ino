#include <iostream>
#include <string>
#include <stdio.h>
#include <LinkedList.h>
#include <Wire.h>
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
#include "SD_funcs.h"
#include "ESP32Time.h"
//#include "time.h"
//#include <sys/time.h>

#include <RGBmatrixPanel.h>

#define SCK_SD   18
#define MOSI_SD  23
#define MISO_SD  19
#define CS_SD    5
//#define powr_but 1
#define CLK  15 //Initialize the IO pin value
#define OE   33
#define LAT 32
#define A   12
#define B   16
#define C   17
#define D   2
TwoWire I2C_32 = TwoWire(0);

//from but_funcs.cpp
//#define powr_but 35

struct Button {
  const uint8_t PIN;
  int numberKeyPresses;
  bool pressed;
  bool hold;
  bool check;
  bool cur_mode;
};
Button mode_but = {34, 0, false, false, false, false};
Button powr_but = {35, 0, false, false, false, false};

int sleep_last = 1;
hw_timer_t * but_timer = NULL;


SPIClass spi_SD = SPIClass(VSPI);
int* displayequalizer(int fft[16],void* current);
int fftconvert(int fft);
void displaysetup(int error);
void extractfilename();
int checkbutton(int set);
void GetImgAndDisp(const char* filename);
int displayimage(int arr[32][32][3], int height, int width);
void displaytime(int timedata[2], int set);
void draw0(int digit[2], int color);
void draw1(int digit[2], int color);
void draw2(int digit[2], int color);
void draw3(int digit[2], int color);
void draw4(int digit[2], int color);
void draw5(int digit[2], int color);
void draw6(int digit[2], int color);
void draw7(int digit[2], int color);
void draw8(int digit[2], int color);
void draw9(int digit[2], int color);
void segD(int coor[2], int color);
void segA(int coor[2], int color);
void segG(int coor[2], int color);
void segC(int coor[2], int color);
void segB(int coor[2], int color);
void segE(int coor[2], int color);
void segF(int coor[2], int color);
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
ESP32Time rtc(0);

// init timer type
hw_timer_t * image_timer = NULL; // "image_timer" is name
// global timer variables
volatile int disp_new_image = 1;
// timer function
void ARDUINO_ISR_ATTR change_image() {
  disp_new_image = 1;
}

void wait(unsigned long duration){
  //This function cause the microcontroller to wait for a certain amount of millisecond before continueing
  unsigned long current_time = millis();
  while(millis() < current_time + duration){  
  }
}

int compare(char *&a, char *&b) {
  return strcmp(a, b);
}

void IRAM_ATTR powr_isr() {
  if (powr_but.hold) {
    powr_but.hold = false;
  }
  else if (digitalRead(powr_but.PIN)) {
    powr_but.check = true;
  }
  else {
    if (powr_but.check) {
      esp_deep_sleep_start();
    }
  }
}

void IRAM_ATTR mode_isr() {
  if (digitalRead(mode_but.PIN) == HIGH) {
    mode_but.check = true;
    timerStart(but_timer);
  }
  else {
    mode_but.check = false;
    // hold
    if (timerRead(but_timer) > 2000) {
      mode_but.hold = true;
    }
    // press
    else {
      mode_but.pressed = true;
    }
    timerStop(but_timer);
    timerWrite(but_timer, 0);
  }
}

void mode_pressed(int* set, int* hour, int* minute) {
  int mode_state = 0;
  int cur_time[2] = {rtc.getHour(true), rtc.getMinute()};

  if (mode_but.pressed) {
    mode_but.pressed = false;
    mode_but.hold = false;
    mode_state = mode_but.numberKeyPresses;
    Serial.print("\nPressed: ");
    if (mode_state == 0) {
      Serial.print("Mode change: ");
      mode_but.cur_mode = !mode_but.cur_mode;
      Serial.print(mode_but.cur_mode);
      if (mode_but.cur_mode == 0){
        *set = 5;
      }
      else{
        *set = 10;
      }
    }
    else{
      Serial.print("Time change:");
      //int cur_time[2] = {rtc.getHour(true), rtc.getMinute()};
      int setmode = 0;
      //int cur_min = rtc.getMinute();
      //int cur_hrs = rtc.getHour(true);

      if (mode_state == 3) {
        Serial.print("Hrs change: ");
        cur_time[0] = cur_time[0] + 1;
        *set = 3;
      }
      else if (mode_state == 2) {
        Serial.print("10Min change: ");
        cur_time[1] = cur_time[1] + 10;
        *set = 2;
      }
      else {
        Serial.print("Min change: ");
        cur_time[1] = cur_time[1] + 1;
        *set = 1;
      }
      if (cur_time[1] >= 60) {
        cur_time[1] = cur_time[1] - 60;
        cur_time[0]++;
      }
      cur_time[0] = cur_time[0] % 24;
      rtc.setTime(rtc.getSecond(), cur_time[1], cur_time[0], 1, 1, 2021);
      Serial.println(rtc.getHour(true)); Serial.print(" : "); Serial.println(rtc.getMinute());
      //displaytime(cur_time, setmode);
    }
    *hour = cur_time[0];
    *minute = cur_time[1];
  }
  else if (mode_but.hold) {
    Serial.print("\nHeld: ");
    mode_but.hold = false;
    mode_but.pressed = false;
    mode_but.numberKeyPresses = (mode_but.numberKeyPresses+1)%4;
    Serial.print(mode_but.numberKeyPresses);
    //displaytime(cur_time, 5 - mode_but.numberKeyPresses);
    if (mode_but.numberKeyPresses == 0){
      *set = 5;
    }
    else{
      *set = mode_but.numberKeyPresses;
    }
    *hour = cur_time[0];
    *minute = cur_time[1];
  }
}

/*void power_button() {

  if (digitalRead(powr_but)) {
    while (digitalRead(powr_but));
    esp_deep_sleep_start();
  }

}*/

void setup_SD(){
  //Set up SPI for SD
  char filename;
  File root;
  spi_SD.begin(SCK_SD, MISO_SD, MOSI_SD, CS_SD);

  // Set up SD
  if(!SD.begin(CS_SD, spi_SD, 80000000)){
    // failed
    Serial.print("SD failed");
    displaysetup(1);
    return;
  }

  // check for SD card
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    // no card
    Serial.print("no card");
    return;
  }
}

// setup code
#define BUFFPIXEL 32
int  disp_arr[32][32][3];
int temp[32][32][3];
int timedata[2];
char* test1;
char test2[50];
const char* temp3;
int newtimetest[2];
int initializetime = 0;
File root;
LinkedList<char*> myFileList = LinkedList<char*>();

void setup()
{
  int j = 0;
  // initialize LED digital pin as an output.
  //pinMode(led21, OUTPUT);
  //pinMode(led23, OUTPUT);
  //pinMode(mode_but, INPUT);
  //pinMode(powr_but, INPUT);
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, HIGH);
  Serial.begin(9600);
  matrix.begin();
  image_timer = timerBegin(0, 80, true); // init timer
  timerAttachInterrupt(image_timer, &change_image, true); // init interrupt for timer
  timerAlarmWrite(image_timer, 5000000, true); // create alarm to call interrupt
  timerAlarmEnable(image_timer); // enable alarm
  pinMode(0, INPUT);
  pinMode(mode_but.PIN, INPUT);
  pinMode(powr_but.PIN, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, HIGH);
  I2C_32.begin(13, 14, 100000);
  attachInterrupt(mode_but.PIN, mode_isr, CHANGE);
  if (digitalRead(powr_but.PIN)) {
    powr_but.hold = true;
  }
  attachInterrupt(powr_but.PIN, powr_isr, CHANGE);
  but_timer = timerBegin(2, 40000, true);
  timerStop(but_timer);
  timerWrite(but_timer, 0);
  displaysetup(0);
  extractfilename();
}
// Main Code
int timetest[2];
int set = 0;
int i = 0;
int re = 0;
int pointer = 1;
int filenum = 0;

const char* filename;
int j = 0;
uint8_t temp_data[16];
int initialfft[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void *current = &initialfft;
uint8_t rec_data;
int fft[16];
  
void loop() {
  if (digitalRead(0) == LOW){
    Serial.println("Phone connected");
    rec_data = I2C_32.requestFrom(0x50, 16, true); //add address
    //Serial.print(rec_data);
    if ((bool)rec_data) {
      //uint8_t temp_data[rec_data];
      I2C_32.readBytes(temp_data, rec_data);
      //log_print_buf(temp_data, rec_data);
    }
    for (int u = 0; u < 16; u++){
      //Serial.print("temp_data = ");
      //Serial.println((int)temp_data[u]);
      if ((int)temp_data[u] == 106){
        break;
      }
      fft[u] = fftconvert((int)temp_data[u]);
      Serial.print("fft = ");
      Serial.println(fft[u]);
    }
    Serial.println("end of one set");
    current = displayequalizer(fft, current);
  }
  else if(digitalRead(0) == HIGH){
    Serial.println("Phone disconnected");
    set = checkbutton(set);
    if (initializetime == 0){
      timetest[0] = rtc.getHour(true);
      timetest[1] = rtc.getMinute();
      initializetime++;
    }
    if (disp_new_image & (mode_but.numberKeyPresses == 0) & (mode_but.cur_mode == 0)) {
      char*n = myFileList.get(re);
      GetImgAndDisp((const char *) n);
      re++;
      if (re >= myFileList.size()){
        re = 0;
      }
      timerAlarmDisable(image_timer);
      timerStop(image_timer);
      if (n[1] == 'g'){
        timerAlarmWrite(image_timer, 1000, true);
        Serial.println("timer set for gif");
      }
      else if(n[1] == 'i'){
        timerAlarmWrite(image_timer, 5000000, true);
        Serial.println("timer set for image");
      }
      timerAlarmEnable(image_timer);
      timerRestart(image_timer);
      timerStart(image_timer);
      disp_new_image = 0;
    }
    else if ((set == 10) & ((timetest[0] != rtc.getHour(true)) | (timetest[1] != rtc.getMinute()))){
      Serial.println("In the time update");
      timetest[0] = rtc.getHour(true);
      timetest[1] = rtc.getMinute();
      displaytime(timetest, 0);
    }
  }
}

int* displayequalizer(int fft[16], void *current){
  int *intCurr = (int *) current;
  int pointer[2] = {0, 31};
  static int location[16];
  int x, y, current_height, new_height;
  int width = 2;
  int numbin = 16;
  int r, g, b;
  int color[16][3] = {{255, 0, 0},
    {255, 96, 0},
    {255, 192, 0},
    {222, 255, 0},
    {126, 255, 0},
    {30, 255, 0},
    {0, 255, 66},
    {0, 255, 162},
    {0, 252, 255},
    {0, 162, 255},
    {0, 66, 255},
    {30, 0, 255},
    {126, 0, 255},
    {222, 0, 255},
    {255, 0, 192},
    {255, 0, 96}};

  matrix.fillScreen(matrix.Color888(0, 0, 0));
  for (int i = 0; i < numbin; i++){
    new_height = fft[i];
    current_height = *(intCurr + i);
    if (new_height != 0) {
      x = pointer[0];
      y = pointer[1] - new_height + 1;
      r = color[i][0];
      g = color[i][1];
      b = color[i][2];
      matrix.drawRect(x, y, width, new_height, matrix.Color888(r, g, b));
      //pointer[0] = pointer[0] + 2;
    }
    /*else {
      pointer[0] = pointer[0] + 2;
    }*/
    /*if (new_height >= current_height) {
      y = pointer[1] - new_height - 1;
      matrix.drawRect(x, y, width, 2, matrix.Color888(255, 255, 255));
      
    }
    else {
      y = pointer[1] - current_height + 1;
      //y = 31 - y;
      matrix.drawRect(x, y, width, 2, matrix.Color888(255, 255, 255));
    }*/
    location[i] = 31 - y;
    pointer[0] = pointer[0] + 2;
  /*Serial.print("current_height =  ");
  Serial.println(current_height);
  }
  for (int i = 0; i < 16; i++) {
    Serial.print("height =  ");
    Serial.println(location[i]);*/
  }
  return location;
}

int fftconvert(int inputdata){
  double maxout = 40;
  double range = 30 / maxout;
  double fft = inputdata;
  Serial.print("range = ");
  Serial.println(range);
  Serial.print("raw fft = ");
  Serial.println(fft);
  if (fft < range){
    return(0);
  }
  else if ((fft >= range) & (fft < range * 2)){
    return(1);
  }
  else if ((fft >= range * 2) & (fft < range * 3)){
    return(2);
  }
  else if ((fft >= range * 3) & (fft < range * 4)){
    return(3);
  }
  else if ((fft >= range * 4) & (fft < range * 5)){
    return(4);
  }
  else if ((fft >= range * 5) & (fft < range * 6)){
    return(5);
  }
  else if ((fft >= range * 6) & (fft < range * 7)){
    return(6);
  }
  else if ((fft >= range * 7) & (fft < range * 8)){
    return(7);
  }
  else if ((fft >= range * 8) & (fft < range * 9)){
    return(8);
  }
  else if ((fft >= range * 9) & (fft < range * 10)){
    return(9);
  }
  else if ((fft >= range * 10) & (fft < range * 11)){
    return(10);
  }
  else if ((fft >= range * 11) & (fft < range * 12)){
    return(11);
  }
  else if ((fft >= range * 12) & (fft < range * 13)){
    return(12);
  }
  else if ((fft >= range * 13) & (fft < range * 14)){
    return(13);
  }
  else if ((fft >= range * 14) & (fft < range * 15)){
    return(14);
  }
  else if ((fft >= range * 15) & (fft < range * 16)){
    return(15);
  }
  else if ((fft >= range * 16) & (fft < range * 17)){
    return(16);
  }
  else if ((fft >= range * 17) & (fft < range * 18)){
    return(17);
  }
  else if ((fft >= range * 18) & (fft < range * 19)){
    return(18);
  }
  else if ((fft >= range * 19) & (fft < range * 20)){
    return(19);
  }
  else if ((fft >= range * 20) & (fft < range * 21)){
    return(20);
  }
  else if ((fft >= range * 21) & (fft < range * 22)){
    return(21);
  }
  else if ((fft >= range * 22) & (fft < range * 23)){
    return(22);
  }
  else if ((fft >= range * 23) & (fft < range * 24)){
    return(23);
  }
  else if ((fft >= range * 24) & (fft < range * 25)){
    return(24);
  }
  else if ((fft >= range * 25) & (fft < range * 26)){
    return(25);
  }
  else if ((fft >= range * 26) & (fft < range * 27)){
    return(26);
  }
  else if ((fft >= range * 27) & (fft < range * 28)){
    return(27);
  }
  else if ((fft >= range * 28) & (fft < range * 29)){
    return(28);
  }
  else if ((fft >= range * 29) & (fft < range * 30)){
    return(29);
  }
  else if (fft >= range * 30){
    return(0);
  }
}

void displaysetup(int error){
  
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // draw some text!
  matrix.setCursor(1, 0);    // start at top left, with one pixel of spacing
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves
  matrix.setTextColor(matrix.Color333(7,7,7));
  if (error == 0){
    matrix.println("Load");
    matrix.println("ing");
    matrix.println("SD");
    matrix.println("Card");
  }
  else if (error == 1){
    matrix.println("SD");
    matrix.println("Card");
    matrix.println("Not");
    matrix.println("Found");
  }
  else if (error == 2){
    matrix.println("Fatal");
    matrix.println("Error");
    matrix.println("Please");
    matrix.println("Reset");
  }
}

void extractfilename(){
  char* test1;
  char test2[50];
  const char* temp3;
  File root;
  setup_SD();
  Serial.println("Setup success");
  root = SD.open("/");
  Serial.println("Root created");
  while (true) {
    File entry =  root.openNextFile();
    if (j == 0) {
      entry =  root.openNextFile();
      j++;
    }
    Serial.println("Entry created");
    if (! entry) {
      // no more files
      Serial.println("Enter if condition");
      Serial.println("end of the files");
      break;
    }
    else {
      Serial.println("Enter else condition");
      char format[50] = "/";
      char* holder;
      char *finalname =(char *) malloc(100);
      temp3 = entry.name();
      //Serial.println(temp3);
      test1 = (char*)temp3;
      strcpy(test2, test1);
      holder = strcat(format, test2);
      Serial.print("holder = ");
      //holder->name = format;
      Serial.println(holder);
      strcpy(finalname, holder);
      myFileList.add(finalname);
      }
      entry.close();
    }
    spi_SD.end();
    myFileList.sort(compare);
}

int checkbutton(int set){
  
  int timetest[2];
  int newset;
  int n = 0;
  
  /*if (sleep_last) {
    sleep_last = digitalRead(powr_but);
  }
  else {
    //Serial.println("System turn off");
    //power_button();
  }*/

  if ((mode_but.pressed) | (mode_but.hold)) {
    mode_pressed(&set, &timetest[0], &timetest[1]);
    /*Serial.print("Set = ");
    Serial.println(set);
    Serial.print("hour = ");
    Serial.println(timetest[0]);
    Serial.print("minute = ");
    Serial.println(timetest[1]);*/
    if (set == 10){
      displaytime(timetest, 0);
    }
    else if (set == 1){
      displaytime(timetest, 4);
    }
    else if (set == 2){
      displaytime(timetest, 3);
    }
    else if (set == 3){
      displaytime(timetest, 2);
    }
  }
  newset = set;
  return (newset);
}

void GetImgAndDisp(const char* filename){
  int error;
  setup_SD();
  error = bmpDraw(SD, filename, disp_arr);
  if (error == 1){
    displaysetup(2);
  }
  else {
    displayimage(disp_arr, 32, 32);
  }
  spi_SD.end();
  return;
}

int displayimage(int arr[32][32][3], int height, int width){
  //This function will extract the RGB value from each pixel of the image and map those RGB value to the LED matrix
  //unsigned long current_time;
  unsigned long pause = 100;
  int *intArr = (int *) arr;
  int frame = height / 32;
  for(int startindex = 0; startindex < frame; startindex++){
    for (int i = 0; i < 32; i++){
      for(int j = 0; j < 32; j++){
        int r = arr[i][j][0];
        int g = arr[i][j][1];
        int b = arr[i][j][2];
        matrix.drawPixel(j, i, matrix.Color888(r, g, b)); //Set the RGB value for each pixel
      }
    }
  }
  return 0;
}

void displaytime(int timedata[2], int set){
  //This function takes in the time data and display it on the screen
  int hour1 = timedata[0] / 10; //Extract each digit from the data
  int hour2 = timedata[0] % 10;
  int minute1 = timedata[1] / 10;
  int minute2 = timedata[1] % 10;
  int digit1[2] = {6, 20}; //Initialize the position of the number on the screen
  int digit2[2] = {13, 20};
  int digit3[2] = {23, 20};
  int digit4[2] = {30, 20};
  int color1 = 0;
  int color2 = 0;
  int color3 = 0;
  int color4 = 0;
  unsigned long pause = 5000;
  if (set == 1) {
    color1 = 1;
  }
  else if (set == 2) {
    color1 = 1;
    color2 = 1;
  }
  else if (set == 3) {
    color3 = 1;
  }
  else if (set == 4) {
    color4 = 1;
  }
  
  matrix.fillRect(0 , 9, 32, 13, matrix.Color888(0, 0, 0)); //Put a black rectangle on the screen
  matrix.fillRect(15, 12, 2, 2, matrix.Color888(255, 255, 255)); //Put the two dots that separate the hour and minute
  matrix.fillRect(15, 17, 2, 2, matrix.Color888(255, 255, 255));
  if (hour1 == 0){ //Logic to assigne the printed value to the input data
    draw0(digit1, color1);
  }
  else if (hour1 == 1){
    draw1(digit1, color1);
  }
  else if (hour1 == 2){
    draw2(digit1, color1);
  }
  else if (hour1 == 3){
    draw3(digit1, color1);
  }
  else if (hour1 == 4){
    draw4(digit1, color1);
  }
  else if (hour1 == 5){
    draw5(digit1, color1);
  }
  else if (hour1 == 6){
    draw6(digit1, color1);
  }
  else if (hour1 == 7){
    draw7(digit1, color1);
  }
  else if (hour1 == 8){
    draw8(digit1, color1);
  }
  else {
    draw9(digit1, color1);
  }

  if (hour2 == 0){
    draw0(digit2, color2);
  }
  else if (hour2 == 1){
    draw1(digit2, color2);
  }
  else if (hour2 == 2){
    draw2(digit2, color2);
  }
  else if (hour2 == 3){
    draw3(digit2, color2);
  }
  else if (hour2 == 4){
    draw4(digit2, color2);
  }
  else if (hour2 == 5){
    draw5(digit2, color2);
  }
  else if (hour2 == 6){
    draw6(digit2, color2);
  }
  else if (hour2 == 7){
    draw7(digit2, color2);
  }
  else if (hour2 == 8){
    draw8(digit2, color2);
  }
  else {
    draw9(digit2, color2);
  }

  if (minute1 == 0){
    draw0(digit3, color3);
  }
  else if (minute1 == 1){
    draw1(digit3, color3);
  }
  else if (minute1 == 2){
    draw2(digit3, color3);
  }
  else if (minute1 == 3){
    draw3(digit3, color3);
  }
  else if (minute1 == 4){
    draw4(digit3, color3);
  }
  else if (minute1 == 5){
    draw5(digit3, color3);
  }
  else if (minute1 == 6){
    draw6(digit3, color3);
  }
  else if (minute1 == 7){
    draw7(digit3, color3);
  }
  else if (minute1 == 8){
    draw8(digit3, color3);
  }
  else {
    draw9(digit3, color3);
  }

  if (minute2 == 0){
    draw0(digit4, color4);
  }
  else if (minute2 == 1){
    draw1(digit4, color4);
  }
  else if (minute2 == 2){
    draw2(digit4, color4);
  }
  else if (minute2 == 3){
    draw3(digit4, color4);
  }
  else if (minute2 == 4){
    draw4(digit4, color4);
  }
  else if (minute2 == 5){
    draw5(digit4, color4);
  }
  else if (minute2 == 6){
    draw6(digit4, color4);
  }
  else if (minute2 == 7){
    draw7(digit4, color4);
  }
  else if (minute2 == 8){
    draw8(digit4, color4);
  }
  else {
    draw9(digit4, color4);
  }
  //wait(pause);
}

void draw8(int digit[2], int color){
  //Draw 8 on the screen
  segD(digit, color);
  segA(digit, color);
  segG(digit, color);
  segC(digit, color);
  segB(digit, color);
  segE(digit, color);
  segF(digit, color);
}

void draw0(int digit[2], int color){
  //Draw 0 on the screen
  segA(digit, color);
  segB(digit, color);
  segC(digit, color);
  segD(digit, color);
  segE(digit, color);
  segF(digit, color);
}

void draw1(int digit[2], int color){
  //Draw 1 on the screen
  segB(digit, color);
  segC(digit, color);
}

void draw2(int digit[2], int color){
  //Draw 2 on the screen
  segA(digit, color);
  segB(digit, color);
  segD(digit, color);
  segE(digit, color);
  segG(digit, color);
}

void draw3(int digit[2], int color){
  //Draw 3 on the screen
  segA(digit, color);
  segB(digit, color);
  segC(digit, color);
  segD(digit, color);
  segG(digit, color);
}

void draw4(int digit[2], int color){
  //Draw 4 on the screen
  segB(digit, color);
  segC(digit, color);
  segF(digit, color);
  segG(digit, color);
}

void draw5(int digit[2], int color){
  //Draw 5 on the screen
  segA(digit, color);
  segC(digit, color);
  segD(digit, color);
  segF(digit, color);
  segG(digit, color);
}

void draw6(int digit[2], int color){
  //Draw 6 on the screen
  segA(digit, color);
  segC(digit, color);
  segD(digit, color);
  segE(digit, color);
  segF(digit, color);
  segG(digit, color);
}

void draw7(int digit[2], int color){
  //Draw 7 on the screen
  segA(digit, color);
  segB(digit, color);
  segC(digit, color);
}

void draw9(int digit[2], int color){
  //Draw 9 on the screen
  segA(digit, color);
  segB(digit, color);
  segC(digit, color);
  segD(digit, color);
  segF(digit, color);
  segG(digit, color);
}

//Set up the segment to represent a number. The display will be 7 segments display
void segD(int coor[2], int color){
  int x = coor[0] - 4;
  int y = coor[1];
  if (color == 1) {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segA(int coor[2], int color){
  int x = coor[0] - 4;
  int y = coor[1] - 10;
  if (color == 1) {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segG(int coor[2], int color){
  int x = coor[0] - 4;
  int y = coor[1] - 5;
  if (color == 1) {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastHLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segC(int coor[2], int color){
  int x = coor[0];
  int y = coor[1] - 4;
  if (color == 1) {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segB(int coor[2], int color){
  int x = coor[0];
  int y = coor[1] - 9;
  if (color == 1) {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segE(int coor[2], int color){
  int x = coor[0] - 5;
  int y = coor[1] - 4;
  if (color == 1) {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}

void segF(int coor[2], int color){
  int x = coor[0] - 5;
  int y = coor[1] - 9;
  if (color == 1) {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 0, 0));
  }
  else {
    matrix.drawFastVLine(x, y, 4, matrix.Color888(255, 255, 255));
  }
}
