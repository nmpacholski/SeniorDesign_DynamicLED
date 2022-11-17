// init timer type
hw_timer_t * image_timer = NULL; // "image_timer" is name
// global timer variables
volatile int disp_new_image = 0;
// timer function
void ARDUINO_ISR_ATTR change_image() {
  disp_new_image = 1;
}

// timer code inside "setup()"
void setup() {
  image_timer = timerBegin(0, 80, true); // init timer
  timerAttachInterrupt(image_timer, &change_image, true); // init interrupt for timer
  timerAlarmWrite(image_timer, 100000, true); // create alarm to call interrupt
  timerAlarmEnable(image_timer); // enable alarm
}

// Example code inside "loop()"
void loop() {
  // function to check if interrupt happened
  if (disp_new_image) {
    /*
      your code here
    */
    disp_new_image = 0;
  }
}
