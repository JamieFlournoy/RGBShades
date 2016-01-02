// Assorted useful functions and variables

boolean effectInit = false; // indicates if a pattern has been recently switched
uint16_t effectDelay = 0; // time between automatic effect changes
unsigned long effectMillis = 0; // store the time of last effect function run
unsigned long cycleMillis = 0; // store the time of last effect change
unsigned long currentMillis; // store current loop's millis value
unsigned long hueMillis; // store time of last hue change
unsigned long clockMillis;
//time_t t;
String timeString = "0000";

byte currentEffect = 0; // index to the currently running effect
boolean autoCycle = false; // flag for automatic effect changes
byte savedCurrentEffect = 0; // currentEffect, saved at the top of the minute.

#define NORMAL_TIME 0
#define COUNTDOWN_TIME 1
#define CONFETTI_TIME 2
byte specialTimeCode = NORMAL_TIME;

boolean specialTimeTestMode = false; // activate specialTimeCode effects every minute instead of every day

CRGBPalette16 currentPalette(RainbowColors_p); // global pallete storage


typedef void (*functionList)(); // definition for list of effect function pointers
#define numEffects (sizeof(effectList) / sizeof(effectList[0]))


// Increment the global hue value for functions that use it
byte cycleHue = 0;
byte cycleHueCount = 0;
void hueCycle(byte incr) {
    cycleHueCount = 0;
    cycleHue+=incr;
}

// Set every LED in the array to a specified color
void fillAll(CRGB fillColor) {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = fillColor;
  }
}

// Fade every LED in the array by a specified amount
void fadeAll(byte fadeIncr) {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = leds[i].fadeToBlackBy(fadeIncr);
  }
}

// Shift all pixels by one, right or left (0 or 1)
void scrollArray(byte scrollDir) {
    byte scrollX = 0;
    for (byte x = 1; x < kMatrixWidth; x++) {
      if (scrollDir == 0) {
        scrollX = kMatrixWidth - x;
      } else if (scrollDir == 1) {
        scrollX = x - 1;
      }

      for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XY(scrollX,y)] = leds[XY(scrollX + scrollDir*2 - 1,y)];
      }
    }

}


// Pick a random palette from a list
void selectRandomPalette() {

  switch(random8(8)) {
    case 0:
    currentPalette = CloudColors_p;
    break;

    case 1:
    currentPalette = LavaColors_p;
    break;

    case 2:
    currentPalette = OceanColors_p;
    break;

    case 4:
    currentPalette = ForestColors_p;
    break;

    case 5:
    currentPalette = RainbowColors_p;
    break;

    case 6:
    currentPalette = PartyColors_p;
    break;

    case 7:
    currentPalette = HeatColors_p;
    break;
  }

}

// Interrupt normal operation to indicate that auto cycle mode has changed
void confirmBlink() {

  if (autoCycle) { // one blue blink, auto mode active
    fillAll(CRGB::DarkBlue);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  } else { // two red blinks, manual mode active
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  }

}

#define NORMAL_TIME 0
#define COUNTDOWN_TIME 1
#define CONFETTI_TIME 2

void setSpecialTimeCode(time_t t) {
  // In specialTimeTestMode, Countdown triggers every
  // minute from :50-00; Confetti triggers every minute from :00-:03.
  boolean topOfMinute = second(t) < 3;
  boolean endOfMinute = second(t) > 50;

  // In !specialTimeTestMode, countdown is 23:50:00-midnight and confetti
  // is from 00:00:00 to 00:00:59.
  boolean firstMinuteOfDay = hour(t) == 0 && minute(t) == 0;
  boolean lastTenMinutesOfDay = hour(t) == 23 && minute(t) >= 50;

  if (firstMinuteOfDay || (specialTimeTestMode && topOfMinute)) {
    specialTimeCode = CONFETTI_TIME;
    return;
  }

  if (lastTenMinutesOfDay || (specialTimeTestMode && endOfMinute)) {
    specialTimeCode = COUNTDOWN_TIME;
    return;
  }

  specialTimeCode = NORMAL_TIME;
}

void padAndAddToTimeString(byte value) {
  if (value < 10) timeString += "0";
  timeString += String(value);
}

void formatTimeString(time_t t) {
  timeString = "";

  if (specialTimeCode == COUNTDOWN_TIME) {
    padAndAddToTimeString(59 - minute(t));
    padAndAddToTimeString(59 - second(t));
    return;
  }

  padAndAddToTimeString(hourFormat12(t));
  padAndAddToTimeString(minute(t));
}

