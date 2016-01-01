//   RGB Shades Demo Code
//   Copyright (c) 2014 macetech LLC
//   This software is provided under the MIT License (see license.txt)
//   Special credit to Mark Kriegsman for XY mapping code
//
//   DEMO: Attach ChronoDot to A4 and A5, display the current RTC time as a scrolling message
//
//   Use Version 3.0 or later https://github.com/FastLED/FastLED
//   ZIP file https://github.com/FastLED/FastLED/archive/master.zip
//
//   Use Arduino IDE 1.0 or later
//
//   If your RGB Shades were purchased before July 2015:
//     This version has the standard Arduino bootloader. R9 and R10 near the control buttons will be present.
//     Select the “Arduino Pro or Pro Mini” option.
//     Then, go back into the Tools menu and find the Processor option and select “ATmega328 (5V, 16MHz)”.
//
//   If your RGB Shades were purchased after July 2015:
//     This version has the Optiboot bootloader. R9 and 10 near the control buttons will be missing.
//     Select the “Arduino Mini” option.
//     Then, go back into the Tools menu and find the Processor option and select “ATmega328”.
//
//   [Press] the SW1 button to cycle through available effects
//   Effects will also automatically cycle at startup
//   [Press and hold] the SW1 button (one second) to switch between auto and manual mode
//     * Auto Mode (one blue blink): Effects automatically cycle over time
//     * Manual Mode (two red blinks): Effects must be selected manually with SW1 button
//
//   [Press] the SW2 button to cycle through available brightness levels
//   [Press and hold] the SW2 button (one second) to reset brightness to startup value


// RGB Shades data output to LEDs is on pin 5
#define LED_PIN  5

// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 72
#define STARTBRIGHTNESS 127
byte currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS

#include <Time.h> // Time Librarycu
#include <Wire.h> // Wire for RTC
#include <DS1307RTC.h> // DS3231/ChronoDot works like DS1307

//Use the example program TimeRTCSet (under file->examples->Time)
// and date +T%s >  /dev/cu.usbserial-DN00M2LG

// Include FastLED library and other useful files
#include <FastLED.h>
#include "messages.h"
// #include "font.h"
#include "sevenseg.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"
#include "buttons.h"


// Runs one time at the start of the program (power up or reset)
void setup() {
  Serial.begin(9600); // set baud to 9600

  Serial.println("Hello world!");

  // write FastLED configuration data
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);//.setCorrection(TypicalSMD5050);

  // set global brightness value
  FastLED.setBrightness( scale8(STARTBRIGHTNESS, MAXBRIGHTNESS) );

  // configure input buttons
  pinMode(MODEBUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);

  setSyncProvider(RTC.get); // set sync to use the ChronoDot
  setSyncInterval(10); // sync every 10 seconds if possible

  // check whether sync worked
  if(timeStatus()!= timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
}

// list of functions that will be displayed
functionList effectList[] = {
  showMinutesAndSecondsSolidColor,
//  showMinutesAndSecondsRainbow,
  // these are activated by special times from the RTC
  // plasma,
  // glitter,
  confetti
};
#define TIMER_INDEX 0
#define CONFETTI_INDEX 1

// Timing parameters
#define cycleTime 15000
#define hueTime 30

// Runs over and over until power off or reset
void loop()
{
  currentMillis = millis(); // save the current timer value
  random16_add_entropy(currentMillis % 37);
  updateButtons(); // read, debounce, and process the buttons

  // Check the mode button (for switching between effects)
  switch(buttonStatus(0)) {

    case BTNRELEASED: // button was pressed and released quickly
      cycleMillis = currentMillis;
      if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
    break;

    case BTNLONGPRESS: // button was held down for a while
      autoCycle = !autoCycle; // toggle auto cycle mode
      confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
    break;

  }

  // Check the brightness adjust button
  switch(buttonStatus(1)) {

    case BTNRELEASED: // button was pressed and released quickly
      currentBrightness += 51; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(currentBrightness,MAXBRIGHTNESS));
    break;

    case BTNLONGPRESS: // button was held down for a while
      currentBrightness = STARTBRIGHTNESS; // reset brightness to startup value
      FastLED.setBrightness(scale8(currentBrightness,MAXBRIGHTNESS));
    break;

  }

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis;
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
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
    effectList[currentEffect](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);

  FastLED.show(); // send the contents of the led memory to the LEDs

  if (currentMillis - clockMillis > 1000) {
    clockMillis = currentMillis;
    time_t t = now();
    setSpecialTimeCode(t);
    formatTimeString(t);

    switch (specialTimeCode) {
      case COUNTDOWN_TIME:
        currentEffect = TIMER_INDEX;
        break;
      case CONFETTI_TIME:
        currentEffect = CONFETTI_INDEX;
        break;
      case NORMAL_TIME:
      default:
        currentEffect = TIMER_INDEX;
    }

    //Serial.begin(9600);
    //Serial.println(timeString);
    //Serial.end();
  }
}

