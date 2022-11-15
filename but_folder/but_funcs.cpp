#define powr_but 12

struct Button {
  const uint8_t PIN;
  int numberKeyPresses;
  bool pressed;
  bool hold;
  bool check;
  bool cur_mode;
};
Button mode_but = {13, 0, false, false, false, false};

int sleep_last = 1;
hw_timer_t * but_timer = NULL;

void power_button() {
  if (digitalRead(powr_but)) {
    while (digitalRead(powr_but));
    esp_deep_sleep_start();
  }
}

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

void mode_pressed() {
  int mode_state = 0;

  if (mode_but.pressed) {
    mode_but.pressed = false;
    mode_state = mode_but.numberKeyPresses;
    //Serial.print("\nPressed: ");
    if (mode_state == 0) {
      //Serial.print("Mode change: ");
      mode_but.cur_mode = !mode_but.cur_mode;
      //Serial.print(mode_but.cur_mode);
    }
    else{
      //Serial.print("Time change:");
      int cur_time[2] = {rtc.getHour(true), rtc.getMinute()};
      //int cur_min = rtc.getMinute();
      //int cur_hrs = rtc.getHour(true);

      if (mode_state == 3) {
        //Serial.print("Hrs change: ");
        cur_time[1] = cur_time[1] + 1;
      }
      else if (mode_state == 2) {
        //Serial.print("10Min change: ");
        cur_time[0] = cur_time[0] + 10;
      }
      else {
        //Serial.print("Min change: ");
        cur_time[0] = cur_time[0] + 1;
      }
      if (cur_time[0] >= 60) {
        cur_time[0] = cur_time[0] - 60;
        cur_time[1]++;
      }
      cur_time[1] = cur_time[1] % 24;
      rtc.setTime(rtc.getSecond(), cur_time[0], cur_time[1], 1, 1, 2021);
      //Serial.println(rtc.getHour(true)); Serial.print(" : "); Serial.println(rtc.getMinute());
    }
  }
  else if (mode_but.hold) {
    //Serial.print("\nHeld: ");
    mode_but.hold = false;
    mode_but.numberKeyPresses = (mode_but.numberKeyPresses+1)%4;
    //Serial.print(mode_but.numberKeyPresses);
  }
}

void setup()
{
  pinMode(mode_but.PIN, INPUT);
  pinMode(powr_but, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, HIGH);

  attachInterrupt(mode_but.PIN, mode_isr, CHANGE);
  but_timer = timerBegin(2, 40000, true);
  timerStop(but_timer);
  timerWrite(but_timer, 0);
}

void loop() {
  // Check power button
  if (sleep_last) {
    sleep_last = digitalRead(powr_but);
  }
  else {
    power_button();
  }

  if ((mode_but.pressed) | (mode_but.hold)) {
    mode_pressed();
  }
}
