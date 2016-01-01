// 3 x 5 pixel numeric font

const char SevenSegFont[][3] PROGMEM = {
{0b00011111, 0b00010001, 0b00011111}, // 0
{0b00000000, 0b00011111, 0b00000000}, // 1
{0b00011101, 0b00010101, 0b00010111}, // 2
{0b00010101, 0b00010101, 0b00011111}, // 3
{0b00000111, 0b00000100, 0b00011111}, // 4
{0b00010111, 0b00010101, 0b00011101}, // 5
{0b00011111, 0b00010101, 0b00011101}, // 6
{0b00000011, 0b00000001, 0b00011111}, // 7
{0b00011111, 0b00010101, 0b00011111}, // 8
{0b00010111, 0b00010101, 0b00011111}, // 9
{0, 0, 0}}; // blank

// starting columns for the 4 characters
// that can fit on the RGBShades display.
const byte SevenSegStartCol[] PROGMEM = {1, 4, 9, 12};

