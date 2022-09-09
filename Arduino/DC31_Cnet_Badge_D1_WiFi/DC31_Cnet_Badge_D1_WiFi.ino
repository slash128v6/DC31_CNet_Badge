//
//   @slash128 2023
//   CompuNet DC31
//
//   Code borrowed from:
//   RGB Shades Demo Code
//   Copyright (c) 2015 macetech LLC
//   This software is provided under the MIT License (see LICENSE)
//   Special credit to Mark Kriegsman for XY mapping code
//   
//   Badge Operation:
//
//   The main power switch selects battery power or USB power
//   USB power will charge the battery
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
//   For WiFi control connect the SSID using the key, both defined in "secrets.h"
//   (recommended to change to something unique)
//   Using a web browser connect to http://192.168.4.1
//   The same functions available using buttons are available on the web page
//
//   Brightness, selected effect, and auto-cycle are saved in EEPROM after a delay
//   The badge will automatically start up with the last-selected settings
//
//   Programming the Badge from the Arduino IDE:
//
//   * Download the badge repo and unzip:
//   * https://github.com/slash128v6/DC31_CNet_Badge
//   * Add the following to "Additional Boards Manager URLs" in preferences:
//   * http://arduino.esp8266.com/stable/package_esp8266com_index.json
//   * Install "esp8266 by ESP8266 Community" version 2.4.1 in Bords Manager
//   * Install the FastLED library version 3.1.6 under "Sketch/Include Library/Manage Libraries" menu
//   * Set the power switch to "USB" and connect to computer via USB
//   * Note the COM port detected in Device Manager
//   * Under the "Tools/Board" menu select the “Wemos D1 R2 & mini” option under "ESP8266 Modules"
//   * Under the "Tools/Port" menu select the COM port detected when plugging in the board to USB
//   * Upload the sketch
//   * Once upload is complete the NeoPixels will start to cycle through the patterns
//

// NeoPixel LED data output to LEDs is on pin 5
#define LED_PIN D3

// NeoPixel LED color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 255
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
#include "patterns.h"
#include "input.h"
#include "secrets.h"

#include <ESP8266WiFi.h>

// list of patterns that will be displayed
functionList patternList[] = { rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle, colorPalette };

const byte numPatterns = (sizeof(patternList)/sizeof(patternList[0]));

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Button variables for webpage
bool autoCycleState = false;
bool brightUp = false;
bool brightDown = false;
bool pattern0State = false;
bool pattern1State = false;
bool pattern2State = false;
bool pattern3State = false;
bool pattern4State = false;
bool pattern5State = false;
bool pattern6State = false;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void checkTime();
void checkButtons();
void checkWiFi();

void setup() {
	// put your setup code here, to run once:

	Serial.begin(115200);

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

	WiFi.softAP(SECRET_SSID, SECRET_PASS); // this is the SSID and password, recommend to change to something else
	server.begin();
}

void loop() {
	// put your main code here, to run repeatedly:

	checkTime();

	checkButtons();

	checkWiFi();


	FastLED.show(); // send the contents of the led memory to the LEDs

}

void checkTime() {
	currentMillis = millis(); // save the current timer value
	updateButtons();          // read, debounce, and process the buttons
	doButtons();              // perform actions based on button state
	checkEEPROM();            // update the EEPROM if necessary
}

void checkButtons() {
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
}

void checkWiFi() {
	WiFiClient client = server.available();   // Listen for incoming clients

	if (client) {                             // If a new client connects,
		Serial.println("New Client.");          // print a message out in the serial port
		String currentLine = "";                // make a String to hold incoming data from the client
		currentTime = millis();
		previousTime = currentTime;
		while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
			currentTime = millis();
			if (client.available()) {             // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);                    // print it out the serial monitor
				header += c;
				if (c == '\n') {                    // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();
						
						// toggles auto-mode and selects the patterns
						if (header.indexOf("GET /autoCycle/on") >= 0) {
							Serial.println("autoCycle on");
							autoCycleState = true;
							} else if (header.indexOf("GET /autoCycle/off") >= 0) {
							Serial.println("autoCycle off");
							autoCycleState = false;
							} else if (header.indexOf("GET /brightUp") >= 0) {
							Serial.println("brightUp pressed");
							brightUp = true;
							} else if (header.indexOf("GET /brightDown") >= 0) {
							Serial.println("brightDown pressed");
							brightDown = true;
							} else if (header.indexOf("GET /pattern0") >= 0) {
							Serial.println("pattern0 pressed");
							pattern0State = true;
							} else if (header.indexOf("GET /pattern1") >= 0) {
							Serial.println("pattern1 pressed");
							pattern1State = true;
							} else if (header.indexOf("GET /pattern2") >= 0) {
							Serial.println("pattern2 pressed");
							pattern2State = true;
							} else if (header.indexOf("GET /pattern3") >= 0) {
							Serial.println("pattern3 pressed");
							pattern3State = true;
							} else if (header.indexOf("GET /pattern4") >= 0) {
							Serial.println("pattern4 pressed");
							pattern4State = true;
							} else if (header.indexOf("GET /pattern5") >= 0) {
							Serial.println("pattern5 pressed");
							pattern5State = true;
							} else if (header.indexOf("GET /pattern6") >= 0) {
							Serial.println("pattern6 pressed");
							pattern6State = true;
						}
						
						// Display the HTML web page
						client.println("<!DOCTYPE html><html>");
						client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<link rel=\"icon\" href=\"data:,\">");
						// CSS to style the buttons
						// Feel free to change the background-color and font-size attributes to fit your preferences
						client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
						client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
						client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
						client.println(".button2 {background-color: #77878A;}</style></head>");
						
						// Web Page Heading
						client.println("<body><h2>NeoPixel WiFi Control</h2>");

						// Display current state for button0 - AutoCycle
						client.println("<p>AutoCycle Mode</p>");
						// If the autoCycleState is off, it displays the OFF button
						if (!autoCycleState) {
							client.println("<p><a href=\"/autoCycle/on\"><button class=\"button button2\">Off</button></a></p>");
							autoCycle = false;
							confirmBlink(); // two green blink: auto mode. two red blinks: manual mode.
							Serial.print("autoCycle = ");
							Serial.println(autoCycle);
							} else if(autoCycleState) {
							client.println("<p><a href=\"/autoCycle/off\"><button class=\"button\">On</button></a></p>");
							autoCycle = true;
							confirmBlink(); // two green blink: auto mode. two red blinks: manual mode.
							Serial.print("autoCycle = ");
							Serial.println(autoCycle);
						}

						
						client.println("<p>Brightness</p>");
						// Display current state for brightUp
						client.println("<p><a href=\"/brightDown\"><button class=\"button\">Down</button></a> <a href=\"/brightUp\"><button class=\"button\">up</button></a></p>");
						
						client.println("<p>Patterns</p>");
						
						// Display current state for pattern0 - Rainbow
						client.println("<p><a href=\"/pattern0\"><button class=\"button\">Rainbow</button></a></p>");
						
						// Display current state for pattern1 - Rainbow with Glitter
						client.println("<p><a href=\"/pattern1\"><button class=\"button\">Rainbow with Glitter</button></a></p>");
						
						// Display current state for pattern2 - Confetti
						client.println("<p><a href=\"/pattern2\"><button class=\"button\">Confetti</button></a></p>");

						// Display current state for pattern3 - Sinelon
						client.println("<p><a href=\"/pattern3\"><button class=\"button\">Sinelon</button></a></p>");

						// Display current state for pattern3 - BPM
						client.println("<p><a href=\"/pattern4\"><button class=\"button\">BPM</button></a></p>");

						// Display current state for pattern5 - Juggle
						client.println("<p><a href=\"/pattern5\"><button class=\"button\">Juggle</button></a></p>");

						// Display current state for pattern6 - Color Palatte
						client.println("<p><a href=\"/pattern6\"><button class=\"button\">Color Palette</button></a></p>");

						if (brightUp) {
							currentBrightness += 51; // increase the brightness (wraps to lowest)
							FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
							eepromMillis = currentMillis;
							eepromOutdated = true;
							} else if (brightDown) {
							currentBrightness -= 51; // increase the brightness (wraps to lowest)
							FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
							eepromMillis = currentMillis;
							eepromOutdated = true;
							} else if (pattern0State) {
							currentPattern = 0;
							} else if (pattern1State) {
							currentPattern = 1;
							} else if (pattern2State) {
							currentPattern = 2;
							} else if (pattern3State) {
							currentPattern = 3;
							} else if (pattern4State) {
							currentPattern = 4;
							} else if (pattern5State) {
							currentPattern = 5;
							} else if (pattern6State) {
							currentPattern = 6;
						}
						Serial.print("Current Pattern: ");
						Serial.println(currentPattern);
						
						brightUp = false;
						brightDown = false;
						pattern0State = false;
						pattern1State = false;
						pattern2State = false;
						pattern3State = false;
						pattern4State = false;
						pattern5State = false;
						pattern6State = false;

						client.println("</body></html>");
						
						// The HTTP response ends with another blank line
						client.println();
						// Break out of the while loop
						break;
						} else { // if you got a newline, then clear currentLine
						currentLine = "";
					}
					} else if (c != '\r') {  // if you got anything else but a carriage return character,
					currentLine += c;      // add it to the end of the currentLine
				}
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		Serial.println("Client disconnected.");
		Serial.println("");
	}
}
