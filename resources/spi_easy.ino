#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Define SPI pins
#define TFT_CS     10
#define TFT_DC     8
#define TFT_RST    9  // You can also use -1 if tied to Arduino RESET

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Define rainbow colors
uint16_t rainbowColors[] = {
  ILI9341_RED,
  ILI9341_ORANGE,
  ILI9341_YELLOW,
  ILI9341_GREEN,
  ILI9341_CYAN,
  ILI9341_BLUE,
  ILI9341_MAGENTA
};

void setup() {
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextSize(3);
  tft.setTextWrap(false);

  int x = 20;
  int y = 100;
  const char* msg = "Hello World";

  for (int i = 0; msg[i] != '\0'; i++) {
    tft.setCursor(x, y);
    tft.setTextColor(rainbowColors[i % 7]);
    tft.print(msg[i]);
    x += 18; // Adjust spacing for size 3 text
  }
}

void loop() {
  // Nothing to do in loop
}
