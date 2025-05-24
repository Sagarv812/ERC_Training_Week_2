# 🧭 Microcontrollers and Peripherals Learning Roadmap
Hello! This covers the necessary topics to learn how to effectively use a microcontroller. Please note that all the code given in the below roadmap is written with Arduino UNO in mind.

- 🌐 [Microcontroller Basics](https://www.geeksforgeeks.org/microcontroller-and-its-types/)
- 🌐 [More in depth basics](https://embedded.fm/blog/2016/2/27/embedded-software-engineering-101-microcontroller-basics)
- 📺 [Video basics](https://youtu.be/JwCTkm43CxQ?si=0tUonzVJ_JYL_ayb)

If you want to skip everything and step right into coding with Arduino, here you go:
  - 📺 [Learn to Code with Arduino](https://youtu.be/BLrHTHUjPuw?si=poh2x0MUSpIbeLGe)

Here's a simulator which you might find useful while learning the coming topics:
- [Wokwi](wokwi.com)
- [Tinkercad](tinkercad.com)

---

## Hardware
This is a collection of resources to fully understand appreciate how an Arduino is built!

Let's start off by seeing what an Arduino consists of:
- [Understanding Hardware](https://www.allaboutcircuits.com/technical-articles/understanding-arduino-uno-hardware-design/)
- [Overview of Components](https://docs.arduino.cc/tutorials/uno-rev3/intro-to-board/)

Now what are peripherals and how do we connect them to an Arduino?
- 🌐 [Peripherals(GPIO)](https://www.renesas.com/en/support/engineer-school/mcu-programming-peripherals-01-gpio?srsltid=AfmBOook9IGdlzMRN3uEcLX7hU-N4Dqs3dF8YJS9AGzY_uVE5iT2rgJh)
- 🌐 [Interfacing Peripherals](https://www.maven-silicon.com/blog/interfacing-peripherals-with-microcontrollers-a-hands-on-guide/)
- 🌐 [Microcontroller peripherals](https://embeddedinventor.com/9-essential-microcontroller-peripherals-explained/)
- 🌐 [Course on embedded systems](https://embedded.fm/blog/ese101)
- 📺 [Detailed Video on GPIOs](https://youtu.be/Naz_qLfcWpg?si=o9IsMjnijAiqaLiN)


---

## Communication
- 🌐 [Protocols](https://medium.com/geekculture/microcontroller-connection-protocols-w1-i2c-spi-uart-7625ad013e60)
- 📖 [More in depth about Communication Protocols](./resources/Communication_Protocols.pdf)
- 🌐 [UART Example](https://docs.arduino.cc/micropython/communication/uart/)
- 🌐 [UART Everything](https://deepbluembedded.com/arduino-uart-example-tutorial/)
- 📺 [Video on UART Communication](https://youtu.be/LubYc87S9tQ?si=WZDt6tsg7JWMZifw)
- 🌐 [I2c, UART, SPI](https://www.parlezvoustech.com/en/comparaison-protocoles-communication-i2c-spi-uart/)

Here is the code for SPI LCD display (easy 🙂):
```
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

```

But if you like to live life dangerously 😉:
```
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

```

---

## MPU6050 IMU
- [Usage](https://randomnerdtutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/)
  
- [Examples](https://electronoobs.com/eng_arduino_tut76.php)

---

## Data Storage
- [EEPROM](https://docs.arduino.cc/learn/built-in-libraries/eeprom/)
- [EEPROM Examples](https://docs.arduino.cc/learn/programming/eeprom-guide/)
- [Flash Memory explained](https://www.youtube.com/watch?v=r2KaVfSH884)
- 
---

## Cores & Threads
- [Stuff](https://www.namehero.com/blog/cpu-cores-vs-threads-everything-you-need-to-know/)
- [Multi Core](https://www.techtarget.com/searchdatacenter/definition/multi-core-processor)
- [Multi Thread](https://www.geeksforgeeks.org/multithreading-in-operating-system/)

---

## RTOS
- [Info](https://www.geeksforgeeks.org/real-time-operating-system-rtos/)

---
Here is the Arduino Uno Pin digram with ample labelling to suit all your needs:
![Arduino Pinout Diagram](./resources/arduino_pin.png "Arduino Pin Diagram")
