//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// RGB Plasma
byte offset  = 0; // counter for radial color wave motion
int plasVector = 0; // counter for orbiting plasma center
void plasma() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}

// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, random8(5) * 63);
    }
  }

}


// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 12; i++) {
    leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255); //CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }

}


// Draw slanting bars scrolling across the array, uses current hue
byte slantPos = 0;
void slantBars() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, quadwave8(x * 32 + y * 32 + slantPos));
    }
  }

  slantPos -= 4;

}

#define NORMAL 0
#define RAINBOW 1

byte randomHueIncrease(byte startHue, byte maxIncrease) {
  byte increase = maxIncrease - random16(maxIncrease/2);
  return (startHue + increase) % 256;
}

void showMinutesAndSeconds(byte style) {
  static byte paletteCycle;
  static byte bitBuffer[16] = {0};
  static boolean havePrintedDebugInfo;

  // static byte secondScaledTo255 = 0; //byte((second()*255)/59);
  static byte hue1;
  static byte hue2;
  static byte hue3;
  static byte hue4;
  static CHSV fg1;
  static CHSV fg2;
  static CHSV fg3;
  static CHSV fg4;

  if (effectInit == false) {
    effectInit = true;
    effectDelay = 100; // ms

    havePrintedDebugInfo = false;

    // zero out the bit buffer
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;

    // Pick 4 different colors for the 4 "seven-segment" display numerals,
    // with a random starting hue and a random 45-135 degree distance
    // between hues.
    hue1 = randomHueIncrease(random16(255), 64);
    hue2 = randomHueIncrease(hue1, 64);
    hue3 = randomHueIncrease(hue2, 64);
    hue4 = randomHueIncrease(hue3, 64);

    fg1 = CHSV(hue1, 255, 255);
    fg2 = CHSV(hue2, 255, 255);
    fg3 = CHSV(hue3, 255, 255);
    fg4 = CHSV(hue4, 255, 255);
  }
  
  paletteCycle += 15; // update the rainbow color

  if (!havePrintedDebugInfo) {
    Serial.print("timeString: ");
    Serial.println(timeString);
  }

  // Fill the bit buffer with seven-segment (3x5) font characters
  // corresponding to the 4-char string in timeString.
  for (byte ch = 0; ch < 4; ch++) {
   	byte bufferIndex = pgm_read_byte(SevenSegStartCol + ch);
		byte charIndex = timeString[ch] - 48;
    for (byte col = 0; col < 3; col++) {
			byte columnBits = pgm_read_byte(SevenSegFont[charIndex] + col);
			bitBuffer[bufferIndex + col] = columnBits;

      if (!havePrintedDebugInfo) {
        Serial.print("bufferIndex: ");
        Serial.println(bufferIndex, DEC);
        Serial.print("charIndex: ");
        Serial.println(charIndex, DEC);
        Serial.print("columnBits: ");
        Serial.println(columnBits, BIN);
      }
    }
  }
  havePrintedDebugInfo = true;

	// Render the bit buffer into the LCD array, mapping 1 bits
	// to the fgColor and 0 bits to the bgColor.
	CRGB pixelColor;
	for (byte displayCol = 0; displayCol < kMatrixWidth; displayCol++) {
    byte colMod4 = byte(displayCol / 4);
    for (byte displayRow = 0; displayRow < 5; displayRow++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[displayCol], displayRow) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle + (displayCol / 4 + displayRow)*16, 255);
        } else {
          CRGB fgColor;
          switch (colMod4) {
            case 0:  fgColor = fg1; break;
            case 1:  fgColor = fg2; break;
            case 2:  fgColor = fg3; break;
            default: fgColor = fg4;
          }
          pixelColor = fgColor;
        }
      } else {
        pixelColor = CRGB::Black;
      }
      leds[XY(displayCol, displayRow)] = pixelColor;
    }
  }
  

}

void showMinutesAndSecondsRainbow() {
  showMinutesAndSeconds(RAINBOW);
}

void showMinutesAndSecondsSolidColor() {
  showMinutesAndSeconds(NORMAL);
}

