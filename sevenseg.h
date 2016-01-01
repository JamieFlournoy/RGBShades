// 3 x 5 pixel numeric font

const char SevenSegFont[][3] PROGMEM = {
{0b00001110, 0b00010001, 0b00001110}, // 0
{0b00010010, 0b00011111, 0b00010000}, // 1
{0b00011001, 0b00010101, 0b00010010}, // 2
{0b00010001, 0b00010101, 0b00001010}, // 3
{0b00000111, 0b00000100, 0b00011111}, // 4
{0b00010111, 0b00010101, 0b00001001}, // 5
{0b00011111, 0b00010101, 0b00011101}, // 6
{0b00000011, 0b00000001, 0b00011111}, // 7
{0b00011111, 0b00010101, 0b00011111}, // 8
{0b00010111, 0b00010101, 0b00011111}, // 9
{0, 0, 0}}; // blank

// starting columns for the 4 characters
// that can fit on the RGBShades display.
const byte SevenSegStartCol[] PROGMEM = {0, 4, 9, 13};

