# Badge Assembly
## Instructions Here or in Word format in the Docs folder
1) Start with the black rear board.

![](images/Picture1.jpg)

2) Solder the power switch. 

![](images/Picture2.jpg)

3)	Solder the battery connector. 

![](images/Picture3.jpg)

4)	Solder the 3 female 2x2 headers. 

![](images/Picture4.jpg)

5)	Solder the microcontroller as shown using 2 1x8 male header pins with the long side of the pins facing up. 

![](images/Picture5.jpg)

6)	Find the 1x12 male header pins. 

![](images/Picture6.jpg)

7)	Cut off a 1x6 section, then cut the remainder into individual pins and set them aside for step 9. 

![](images/Picture7.jpg)

8)	Using the 1x6 section from step 7 solder the battery charger module as shown with the long side of the pins facing up. 

![](images/Picture8.jpg)

9)	Using the 6 single pins from step 7 solder the NeoPixel ring. 

![](images/Picture9.jpg)

10)	Solder the SAO 2x3 header, aligning the notch as shown by the silkscreen.

![](images/Picture10.jpg)

11)	Solder the 2 tactile switches on the back.

![](images/Picture11.jpg)

12)	Use a piece of double-sided tape to mount the battery and connect as shown.

![](images/Picture12.jpg)

13)	Solder the male 2x2 header pins to the rear of the front board. Align to the pads as best as possible so the front board will mate with the rear board. It helps to turn up the iron temp. 

![](images/Picture13.jpg)

14)	Align the front board male header pins with the rear board female headers and push together.  

![](images/Picture14.jpg)

![](images/Picture14a.jpg)

15)	Slide the power switch towards the top of the board (images/USB power) and connect the microcontroller via USB cable. 

![](images/Picture15.jpg)

16)	The charging module will light up showing battery charge level. Once at sufficient charge the power switch can be slid towards the side of the badge for battery power. 

![](images/Picture16.jpg)

If you've made it this far and haven't released any of the secret white smoke, great work! Time to do some programming...

# Software Time!

Programming the Microcontroller

1)	Download the badge repo and unzip:
[https://github.com/slash128v6/DC31_CNet_Badge](images/)

2)	Open the Arduino badge sketch from the Software directory in the Arduino IDE.

3)	Add the following to "Additional Boards Manager URLs" in preferences:
[http://arduino.esp8266.com/stable/package_esp8266c	om_index.json](images/)

![](images/Picture17.png)

4)	Install "esp8266 by ESP8266 Community" version 2.4.1 in Bords Manager.

![](images/Picture18.png)

5)	Install the FastLED library version 3.1.6 under "Sketch/Include Library/Manage Libraries" menu.

![](images/Picture19.png)

6)	Set the power switch towards the top of the board for and connect to computer via USB data cable.

![](images/Picture20.png)

7)	Note the COM port detected in Device Manager and select that port in the Arduino IDE.

![](images/Picture21.png)
 
8)	Select the “Wemos D1 R2 & mini” board and associated COM port.

![](images/Picture22.png)

9)	Upload the sketch. If successful, the NeoPixels will start to cycle through the patterns.

![](images/Picture23.png)

# Operating the Badge
The main power switch selects battery power or USB power. USB power will charge the battery. Slide the power switch towards the top of the badge for USB power or towards the side of the badge for battery power. The battery must be sufficiently charged for battery operation. If the badge does not operate on battery power let it charge via USB. There is a battery “gas gauge” that is viewable from the rear of the board.

![](images/Picture24.jpg)

Press SW1 button to cycle through available effects.
 Press and hold SW1 button for one second to switch between auto and manual mode.
Auto Mode (images/two green blinks): Effects automatically cycle over time per cycleTime.
Manual Mode (images/two red blinks): Effects must be selected manually with left button.

Press SW2 button to cycle through available brightness levels.
Press and hold SW2 button for one second to reset brightness to default value.

For WiFi control connect the SSID using the key, both defined in "secrets.h".
(images/default is CompuNet/compunet but recommended to change to something unique).

Using a web browser connect to [http://192.168.4.1](images/)
The same functions available using buttons are available on the web page.
Brightness, selected effect, and auto-cycle are saved in EEPROM after a delay.
The badge will automatically start up with the last-selected settings.

# Assembling the SAO

1)	The SAO has 3 LEDs and a pin header connector.

![](images/Picture25.jpg)

2)	Solder the 3 LEDs on the back of the SAO and bend the LED leads so they are parallel to the PCB. The short lead goes into the square pad.

![](images/Picture26.jpg)

3)	Solder the connector on the back of the SAO with the notch orientation as indicated.

![](images/Picture27.jpg)
