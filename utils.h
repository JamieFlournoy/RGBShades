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

/*
// Determine flash address of text string
unsigned int currentStringAddress = 0;
void selectFlashString(byte string) {
  currentStringAddress = pgm_read_word(&stringArray[string]);
}

// Fetch font character bitmap from flash
byte charBuffer[5] = {0};
void loadCharBuffer(byte character) {
  byte mappedCharacter = character;
  if (mappedCharacter >= 32 && mappedCharacter <= 95) {
    mappedCharacter -= 32; // subtract font array offset
  } else if (mappedCharacter >= 97 && mappedCharacter <= 122) {
    mappedCharacter -= 64; // subtract font array offset and convert lowercase to uppercase
  } else {
    mappedCharacter = 96; // unknown character block
  }

  for (byte i = 0; i < 5; i++) {
    charBuffer[i] = pgm_read_byte(Font[mappedCharacter]+i);
  }
}

// Fetch a character value from a text string in flash
char loadStringChar(byte string, byte character) {

  if (string == 99) {
    return timeString.charAt(character);
  } else {
    return (char) pgm_read_byte(currentStringAddress + character);
  }
}
*/


/*
void formatTimeString() {
  timeString = "";
  t = now();

  timeString += dayStr(weekday(t));
  timeString += " ";
  timeString += monthShortStr(month(t));
  timeString += " ";
  timeString += String(day(t));
  timeString += " ";

  if (hourFormat12(t) < 10) timeString += "0";
  timeString += String(hourFormat12(t));
  timeString += ":";
  if (minute(t) < 10) timeString += "0";
  timeString += String(minute(t));
  timeString += ":";
  if (second(t) < 10) timeString += "0";
  timeString += String(second(t));
  if (isAM(t)) timeString += " AM  "; else timeString += " PM  ";
}
*/

#define NORMAL_TIME 0
#define COUNTDOWN_TIME 1
#define CONFETTI_TIME 2

void setSpecialTimeCode(time_t t) {
  if (hour(t) == 23 && minute(t) > 50) {
    specialTimeCode = COUNTDOWN_TIME;
    return;
  }
  if (hour(t) == 0 && minute(t) == 0) {
    specialTimeCode = CONFETTI_TIME;
    return;
  }

/*
  // test hack
  if (second(t) < 3) {
    specialTimeCode = CONFETTI_TIME;
    return;
  }
  if (second(t) > 50) {
    specialTimeCode = COUNTDOWN_TIME;
    return;
  }
*/
  specialTimeCode = NORMAL_TIME;
}

void formatTimeString(time_t t) {
  timeString = "";

  //byte hours12 = hourFormat12(t);
  byte minutes = minute(t);
  byte seconds = second(t);

  if (specialTimeCode == COUNTDOWN_TIME) {
    minutes = 60 - minutes;
    seconds = 60 - seconds;
  }

  if (minutes < 10) timeString += "0";
  timeString += String(minutes);

  // timeString += ":";

  if (seconds < 10) timeString += "0";
  timeString += String(seconds);
}

