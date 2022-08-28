// Process input

#define NUMBUTTONS 3
#define MODEBUTTON D5
#define BRIGHTNESSBUTTON D6
#define WIFISERVER D7

#define BTNIDLE 0
#define BTNDEBOUNCING 1
#define BTNPRESSED 2
#define BTNRELEASED 3
#define BTNLONGPRESS 4
#define BTNLONGPRESSREAD 5

#define BTNDEBOUNCETIME 20
#define BTNLONGPRESSTIME 1000

unsigned long buttonEvents[NUMBUTTONS];
byte buttonStatuses[NUMBUTTONS];
byte buttonmap[NUMBUTTONS] = {BRIGHTNESSBUTTON, MODEBUTTON, WIFISERVER};

void updateButtons() {
  for (byte i = 0; i < NUMBUTTONS; i++) {
    switch (buttonStatuses[i]) {
      case BTNIDLE:
        if (digitalRead(buttonmap[i]) == LOW) {
          buttonEvents[i] = currentMillis;
          buttonStatuses[i] = BTNDEBOUNCING;
        }
        break;

      case BTNDEBOUNCING:
        if (currentMillis - buttonEvents[i] > BTNDEBOUNCETIME) {
          if (digitalRead(buttonmap[i]) == LOW) {
            buttonStatuses[i] = BTNPRESSED;
          }
        }
        break;

      case BTNPRESSED:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNRELEASED;
        } else if (currentMillis - buttonEvents[i] > BTNLONGPRESSTIME) {
          buttonStatuses[i] = BTNLONGPRESS;
        }
        break;

      case BTNRELEASED:
        break;

      case BTNLONGPRESS:
        break;

      case BTNLONGPRESSREAD:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNIDLE;
        }
        break;
    }
  }
}

byte buttonStatus(byte buttonNum) {

  byte tempStatus = buttonStatuses[buttonNum];
  if (tempStatus == BTNRELEASED) {
    buttonStatuses[buttonNum] = BTNIDLE;
  } else if (tempStatus == BTNLONGPRESS) {
    buttonStatuses[buttonNum] = BTNLONGPRESSREAD;
  }

  return tempStatus;

}

void doButtons() {
  
  // Check the mode button (for switching between effects)
  switch (buttonStatus(0)) {

    case BTNRELEASED: // button was pressed and released quickly
      cycleMillis = currentMillis;
      if (++currentPattern >= numPatterns) currentPattern = 0; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      autoCycle = !autoCycle; // toggle auto cycle mode
      confirmBlink(); // two green blink: auto mode. two red blinks: manual mode.
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

  }

  // Check the brightness adjust button
  switch (buttonStatus(1)) {

    case BTNRELEASED: // button was pressed and released quickly
      currentBrightness += 51; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      currentBrightness = STARTBRIGHTNESS; // reset brightness to startup value
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

  }

  // Check the wifi server button
  switch (buttonStatus(2)) {

    /**
    case BTNRELEASED: // button was pressed and released quickly
      wifiEnabledFlag = 1; // switch to wifi mode
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;
    **/
    case BTNLONGPRESS: // button was held down for a while
      wifiEnabledFlag = !wifiEnabledFlag; // toggle wifi mode
      //autoCycle = !autoCycle; // toggle auto cycle mode
      wifiBlink(); // two blue blinks: wifi mode enabled. two green blinks: wifi mode disabled.
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

  }

}
