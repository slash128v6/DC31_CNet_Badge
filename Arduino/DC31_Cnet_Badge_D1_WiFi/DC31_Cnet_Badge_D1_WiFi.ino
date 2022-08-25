//
//   
//   @slash128 2023
//   CompuNet DC31
//   
//   
//   Badge Operation:
//   
//   The main power switch selects battery power or USB power
//   
//   Press SW1 button to cycle through available effects
//   
//   Press and hold SW1 button for one second to switch between auto and manual mode
//   
//   * Auto Mode (two green blinks): Effects automatically cycle over time per cycleTime
//   * Manual Mode (two red blinks): Effects must be selected manually with left button
//
//   Press SW2 button to cycle through available brightness levels
//   
//   Press and hold SW2 button for one second to reset brightness to default value
//
//   Brightness, selected effect, and auto-cycle are saved in EEPROM after a delay
//   The badge will automatically start up with the last-selected settings
//   
//   
//   Programming the Badge from the Arduino IDE:
//   
//   * Download the badge repo and unzip in your Arduino sketch folder:
//   * https://github.com/slash128v6/CnetBadge2018DC26
//   * Install the FastLED library under "Sketch/Include Library/Manage Libraries" menu.
//   * Set the power switch to "USB" and connect to computer via USB.
//   * Note the COM port detected in Device Manager.
//   * Under the "Tools/Board" menu select the “Arduino Pro or Pro Mini” option.
//   * Under the "Tools/Processor" menu select “ATmega328 (5V, 16MHz)”.
//   * Under the "Tools/Port" menu select the COM port detected when plugging in the board to USB.
//   * After the sketch compiles it should upload and the "TX/RX" lights will flash.
//   * Once upload is complete the NeoPixels will start to cycle through the patterns.
//

// NeoPixel LED data output to LEDs is on pin 5
#define LED_PIN D5

// NeoPixel LED color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 64
#define STARTBRIGHTNESS 128

// Cycle time (milliseconds between pattern changes)
#define cycleTime 15000

// Hue time (milliseconds between hue increments)
#define hueTime 30

// Time after changing settings before settings are saved to EEPROM
#define EEPROMDELAY 2000

// Includes
#include <FastLED.h>
#include <EEPROM.h>
#include "XYmatrix.h"
#include "utils.h"
#include "colorPalette.h"
// #include "cylon.h"
#include "patterns.h"
#include "input.h"

// list of patterns that will be displayed
functionList patternList[] = { rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle, colorPalette };

const byte numPatterns = (sizeof(patternList)/sizeof(patternList[0]));

void setup() {
  // put your setup code here, to run once:

  // check to see if EEPROM has been used yet
  // if so, load the stored settings
  byte eepromWasWritten = EEPROM.read(0);
  if (eepromWasWritten == 99) {
    currentPattern = EEPROM.read(1);
    autoCycle = EEPROM.read(2);
    currentBrightness = EEPROM.read(3);
  }

  if (currentPattern > (numPatterns - 1)) currentPattern = 0;

  // write FastLED configuration data
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  // set global brightness value
  FastLED.setBrightness( scale8(currentBrightness, MAXBRIGHTNESS) );

  // configure input buttons
  pinMode(MODEBUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);

}


void loop() {
  // put your main code here, to run repeatedly:
  
  currentMillis = millis(); // save the current timer value
  updateButtons();          // read, debounce, and process the buttons
  doButtons();              // perform actions based on button state
  checkEEPROM();            // update the EEPROM if necessary

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis;
    if (++currentPattern >= numPatterns) currentPattern = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    patternList[currentPattern](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  FastLED.show(); // send the contents of the led memory to the LEDs

}
