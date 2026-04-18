// ===============================================================
// TFT_eSPI User_Setup.h for ESP32-S3 + ST7735 1.44" 128x128 LCD
// Copy this file to: Arduino/libraries/TFT_eSPI/User_Setup.h
// ===============================================================

// -- Driver --
#define ST7735_DRIVER
#define TFT_WIDTH  128
#define TFT_HEIGHT 128
#define ST7735_GREENTAB3   // For 1.44" 128x128 screens

#define USE_HSPI_PORT
// -- Pin Definitions (change to match your wiring) --
#define TFT_MOSI 11   // SDA
#define TFT_SCLK 12   // SCL
#define TFT_CS   10   // CS
#define TFT_DC   14   // DC / RS
#define TFT_RST  9   // RST

// -- SPI Speed --
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY   5000000

// -- Color order (swap if colors look wrong) --
// #define TFT_RGB_ORDER TFT_BGR   // Uncomment if red/blue are swapped

// -- Fonts (optional, comment out to save flash) --
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT