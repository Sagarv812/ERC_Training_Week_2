#include <SPI.h>

#define TFT_CS    10
#define TFT_DC    8
#define TFT_RST   9

// Rainbow color palette (RGB565 format)
uint16_t rainbowColors[] = {
  0xF800, // RED
  0xFC00, // ORANGE
  0xFFE0, // YELLOW
  0x07E0, // GREEN
  0x07FF, // CYAN
  0x001F, // BLUE
  0xF81F  // MAGENTA
};

void writeCommand(uint8_t cmd) {
  digitalWrite(TFT_DC, LOW);
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(cmd);
  digitalWrite(TFT_CS, HIGH);
}

void writeData(uint8_t data) {
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(data);
  digitalWrite(TFT_CS, HIGH);
}

void writeData16(uint16_t data) {
  writeData(data >> 8);
  writeData(data & 0xFF);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  writeCommand(0x2A); // Column addr set
  writeData16(x0);
  writeData16(x1);

  writeCommand(0x2B); // Row addr set
  writeData16(y0);
  writeData16(y1);

  writeCommand(0x2C); // Write to RAM
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  setAddrWindow(x, y, x, y);
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(color >> 8);
  SPI.transfer(color & 0xFF);
  digitalWrite(TFT_CS, HIGH);
}

// Simple 5x7 ASCII font (subset for demo)
const uint8_t font5x7[][5] = {
  // Only H, e, l, o, W, r, d characters
  {0x7C, 0x10, 0x10, 0x10, 0x7C}, // H
  {0x20, 0x54, 0x54, 0x54, 0x78}, // e
  {0x7C, 0x08, 0x08, 0x08, 0x70}, // l
  {0x38, 0x44, 0x44, 0x44, 0x38}, // o
  {0x7C, 0x04, 0x08, 0x10, 0x7C}, // W
  {0x38, 0x44, 0x44, 0x28, 0x5C}, // r
  {0x38, 0x54, 0x54, 0x54, 0x18}, // d
};

const char text[] = "Hello World";
const char fontMap[] = "HeloWrd";

int getFontIndex(char c) {
  for (int i = 0; i < sizeof(fontMap) - 1; i++) {
    if (fontMap[i] == c) return i;
  }
  return -1; // Unsupported char
}

void drawChar(int x, int y, char c, uint16_t color) {
  int idx = getFontIndex(c);
  if (idx < 0) return;

  for (int i = 0; i < 5; i++) {
    uint8_t col = font5x7[idx][i];
    for (int j = 0; j < 7; j++) {
      if (col & (1 << j)) {
        drawPixel(x + i, y + j, color);
      }
    }
  }
}

void drawString(int x, int y, const char* str) {
  for (int i = 0; str[i]; i++) {
    uint16_t color = rainbowColors[i % 7];
    drawChar(x + i * 6, y, str[i], color);
  }
}

void ili9341_init() {
  writeCommand(0x01); // Software reset
  delay(1000);
  
  writeCommand(0x28); // Display OFF

  writeCommand(0x3A); // Interface Pixel Format
  writeData(0x55);    // 16 bits/pixel

  writeCommand(0x36); // Memory Access Control
  writeData(0x48);    // MX, BGR

  writeCommand(0x11); // Sleep Out
  delay(120);

  writeCommand(0x29); // Display ON
}

void setup() {
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);

  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(150);

  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  ili9341_init();

  drawString(20, 60, text);
}

void loop() {
  // nothing here
}
