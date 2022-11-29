#include <iostream>
#include <string>
#include <stdio.h>
#include <LinkedList.h>
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
#define powr_but 1
#define CLK  15 //Initialize the IO pin value
#define OE   33
#define LAT 32
#define A   12
#define B   16
#define C   17
#define D   2

//from but_funcs.cpp
#define powr_but 35

struct Button {
  const uint8_t PIN;
  int numberKeyPresses;
  bool pressed;
  bool hold;
  bool check;
  bool cur_mode;
};
Button mode_but = {34, 0, false, false, false, false};

int sleep_last = 1;
hw_timer_t * but_timer = NULL;


SPIClass spi_SD = SPIClass(VSPI);
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

//Link list
class Pun {
  public:
    char *name;
};

/*struct node {
  char *filename;
  struct node *next;
};

struct pun {
  struct node *head;
  struct node *tail;
};

struct pun *create_list(void) {
  struct pun *new_list = (struct pun *) malloc(sizeof(struct pun));
  new_list->head = NULL;
  new_list->tail = NULL;
  return new_list;
}

struct node *create_node(char *name) {
  struct node *new_node = (struct node *) malloc(sizeof(node));
  new_node-> filename = name;
  new_node->next = NULL;
  return new_node;
}

void add_file(struct pun *l, char *name) {
  struct node *new_node = create_node(name);
  if (!l->head) {
    l->head = new_node;
    l->tail = new_node;
  } else {
    l->tail->next = new_node;
    l->tail = new_node;
  }
}*/

/*void power_button() {
  if (digitalRead(powr_but)) {
    while (digitalRead(powr_but));
    esp_deep_sleep_start();
  }
}*/

/*char printDirectory(File dir, int numTabs) {
  
  int i = 0;
  const char* filename;
  while (true) {

    File entry =  dir.openNextFile();

    if (! entry) {

      // no more files

      break;

    }

    for (uint8_t i = 0; i < numTabs; i++) {

      Serial.print('\t');

    }
    if (i != 0) {
      Serial.print(entry.name());
      Serial.print("\n");
      filename = entry.name();
      entry.close();
      return (filename);
    }
    else {
      i++;
    }
    if (entry.isDirectory()) {

      Serial.println("/");

      printDirectory(entry, numTabs + 1);

    } else {

      // files have sizes, directories do not

      Serial.print("\t\t");

      Serial.println(entry.size(), DEC);

    }

  }
}*/

void IRAM_ATTR mode_isr() {
  if (!mode_but.check) {
    mode_but.check = true;
    timerStart(but_timer);
  }
  else {
    mode_but.check = false;
    // hold
    if (timerRead(but_timer) > 4000) {
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

void power_button() {

  if (digitalRead(powr_but)) {
    while (digitalRead(powr_but));
    esp_deep_sleep_start();
  }

}

void setup_SD(){
  //Set up SPI for SD
  char filename;
  File root;
  spi_SD.begin(SCK_SD, MISO_SD, MOSI_SD, CS_SD);

  // Set up SD
  if(!SD.begin(CS_SD, spi_SD, 80000000)){
    // failed
    Serial.print("SD failed");
    return;
  }

  // check for SD card
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    // no card
    Serial.print("no card");
    return;
  }
  //Serial.println("SD successful");
  /*root = SD.open("/");

  filename = printDirectory(root, 0);
  Serial.println(filename);*/
  
}

// setup code
#define BUFFPIXEL 32
int  disp_arr[32][32][3];
int temp[32][32][3];
int timedata[2];
char* test1;
char test2[50];
const char* temp3;
//struct pun *filelist = create_list();
File root;
LinkedList<char*> myFileList = LinkedList<char*>();

void setup()
{
  int j = 0;
  //Pun *holder;
  //struct node *curr = filelist->head;
  /*const char* filename;
  char format[] = "/";
  const char* temp;
  char* test1;
  char test2[50];
  int numfile = 0;
  File root;*/
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
  pinMode(mode_but.PIN, INPUT);
  pinMode(powr_but, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, HIGH);

  attachInterrupt(mode_but.PIN, mode_isr, CHANGE);
  but_timer = timerBegin(2, 40000, true);
  timerStop(but_timer);
  timerWrite(but_timer, 0);

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
      //filelist[j - 1] = (const char*) format;
      //add_file(filelist, format);
      //Serial.println(curr->filename);
      //filenum++;
      /*for (int k = 0; k < myFileList.size(); k++){
      //GetImgAndDisp(filelist[k]);
      //delay(100);
      char* n = myFileList.get(k);
      Serial.println(n);*/
      /*curr = curr->next;
      if (!curr){
        curr = filelist->head;
      }*/
      //delay(500);
      }
      //Serial.println("End of the loop");
      entry.close();
    }
    spi_SD.end();
}
  /*for (int k = 0; k < 10; k++){
    //GetImgAndDisp(filelist[k]);
    //delay(100);
    Serial.println(curr->filename);
    curr = curr->next;
    if (!curr){
      curr = filelist->head;
    }
    delay(500);
  }*/

  //void *point = &disp_arr;
// Main Code
char temp1[] = "/g_rick";
//char temp2[2], temp3;
char filetype[] = ".bmp";
int timetest[2];
int set = 0;
int i = 0;
int re = 0;
//const char* filelist[2];
const char* filename1 = "/test_bmp.bmp";
const char* filename2 = "/i_result.bmp";
int pointer = 1;
int filenum = 0;

const char* filename;
//char format[] = "/";
//const char* temp3;
//char* test1;
//char test2[50];
int j = 0;
//File root;
//struct pun *filelist = create_list();

  
void loop() {
  //struct node *curr = filelist->head;
  /*if (re == 0) {
    setup_SD();
    Serial.print("Setup success");
    root = SD.open("/");
    Serial.print("Root created");
    while (true) {
      File entry =  root.openNextFile();
      Serial.print("Entry created");
      if (! entry) {
        // no more files
        Serial.print("Enter if condition");
        Serial.print("end of the files");
        break;
      }
      else {
        Serial.print("Enter else condition");
        char format[50] = "/";
        temp3 = entry.name();
        test1 = (char*)temp3;
        strcpy(test2, test1);
        strcat(format, test2);
        Serial.print("format = ");
        Serial.println(format);
        //filelist[j - 1] = (const char*) format;
        add_file(filelist, format);
        //filenum++;
        entry.close();
      }
    }
    /*spi_SD.end();
    setup_SD();
    root = SD.open("/");
    filenum = filenum - 1;
    Serial.print("Filenum = ");
    Serial.println(filenum);
    //const char* filelist[filenum];
    while (true) {
      File entry =  root.openNextFile();
      if (! entry) {
        // no more files
        Serial.print("end of the files");
        break;
      }
      else if (j != 0) {
        char format[] = "/";
        temp3 = entry.name();
        test1 = (char*)temp3;
        strcpy(test2, test1);
        strcat(format, test2);
        Serial.print("j = ");
        Serial.println(j);
        Serial.print("temp3 = ");
        Serial.println(temp3);
        Serial.print("test1 = ");
        Serial.println(test1);
        Serial.print("test2 = ");
        Serial.println(test2);
        Serial.print("format = ");
        Serial.println(format);
        //filelist[j - 1] = (const char*) format;
        add_file(filelist, format);
        j++;
        entry.close();
      }
      else {
        j++;
      }
    }
    re++;
    spi_SD.end();
  }*/
  //Pun *n;
  /*for (int k = 0; k < myFileList.size(); k++){
    //GetImgAndDisp(filelist[k]);
    //delay(100);
    char*n = myFileList.get(k);
    Serial.println(n);
    curr = curr->next;
    if (!curr){
      curr = filelist->head;
    }
    delay(500);
  }
  Serial.println("End of the loop");*/
  set = checkbutton(set);
  timetest[0] = rtc.getHour(true);
  timetest[1] = rtc.getMinute();
  if (disp_new_image & (mode_but.numberKeyPresses == 0) & (mode_but.cur_mode == 0)) {
    char*n = myFileList.get(re);
    //Serial.println(n);
    GetImgAndDisp((const char *) n);
    re++;
    if (re >= myFileList.size()){
      re = 0;
    }
    //i++;
    //i = i%2;
    if (n[1] == 'g'){
      disp_new_image = 5000000 - 1000;
    }
    else if(n[1] == 'i'){
      disp_new_image = 0;
    }
    Serial.print("disp_new_image = ");
    Serial.print(disp_new_image);
  }

  // Check power button
  //if (sleep_last) {
  //  sleep_last = digitalRead(powr_but);
  //}
  //else {
  //  power_button();
  //}
}

int checkbutton(int set){
  
  int timetest[2];
  int newset;
  int n = 0;
  
  if (sleep_last) {
    sleep_last = digitalRead(powr_but);
  }
  else {
    power_button();
  }

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
    /*else{
      return;
    }
    while (n != 1){
      if ((mode_but.pressed) | (mode_but.hold)) {
        checkbutton();
        n = 1;
      }
    }*/
  }
  newset = set;
  return (newset);
}

void GetImgAndDisp(const char* filename){
  /*const char* filename;
  char format[] = "/";
  const char* temp;
  char* test1;
  char test2[50];
  int i = 0;
  File root;
  setup_SD();
  root = SD.open("/");

  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      pointer = 1;
      break;
    }
    else if (i == pointer) {
      temp = entry.name();
      test1 = (char*)temp;
      strcpy(test2, test1);
      strcat(format, test2);
      filename = format;
      entry.close();
      pointer++;
      break;
    }
    else {
      i++;
    }
  }*/
  setup_SD();
  bmpDraw(SD, filename, disp_arr);
  displayimage(disp_arr, 32, 32);
  spi_SD.end();
  Serial.println("print successful");
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
        /*int r = *(intArr + (((i * width) + (startindex * 1024) + j) * 3) + 0);
        int g = *(intArr + (((i * width) + (startindex * 1024) + j) * 3) + 1);
        int b = *(intArr + (((i * width) + (startindex * 1024) + j) * 3) + 2);*/
        int r = arr[i][j][0];
        int g = arr[i][j][1];
        int b = arr[i][j][2];
        matrix.drawPixel(j, i, matrix.Color888(r, g, b)); //Set the RGB value for each pixel
      }
    }
    //wait(pause);
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
