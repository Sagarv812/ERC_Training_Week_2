# Arduino Project

## Why this?
Well, it's a space shooter game, who doesn't like it? We felt that it was simnple enough to put together while also being complicated enough to induce learning. It is also very fun to play. Inspired by the space shooter - Galaga - this is made to have fun and learn a whole lot along the way.
### Bill of Materials
| Item | Cost | Link |
| --- | --- | --- |
| Arduino Uno | 195 | [Here](https://robu.in/product/arduino-nano-board-r3-with-ch340-chip-wo-usb-cable-solderedarduino-nano-r3-wo-usb-cable-soldered/?gad_source=1&gad_campaignid=21296336107&gbraid=0AAAAADvLFWcfpR3ko8Q9OLzHBNkhMrRTJ&gclid=CjwKCAjw3MXBBhAzEiwA0vLXQeTpaUv2C_AWv_vWnXh7a2x6_TDckb190d0m9Ib6GNEHH4jFwP5apxoC7f4QAvD_BwE) |
| MPU6050 | 165 | [Here](https://robu.in/product/mpu-6050-gyro-sensor-2-accelerometer/) |
| SSD1306 OLED | 209 | [Here](https://robu.in/product/0-96-inch-128x64-ssd1306-iic-interface-4-pin-oled-module-blue-color-screen/) |
| Push Button | 20 | [Here](https://robu.in/product/0-96-inch-128x64-ssd1306-iic-interface-4-pin-oled-module-blue-color-screen/) |
| Breadboard | 35 | [Here](https://robu.in/product/solderless-400-pin-breadboard-normal-quality-without-packing/) |
| --- | --- |
| Total | 624 |

## Circuit Diagram
![Circuit Diagram](./media/circuit_diagram.png)

## Simulation

https://github.com/user-attachments/assets/4c447f56-73f2-4516-90e0-8ddece679c89

## Code 
```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MPU6050 mpu;

#define FIRE_BUTTON_PIN 2

// Player variables
int playerX = SCREEN_WIDTH / 2;
const int playerY = SCREEN_HEIGHT - 8;  // near bottom
const int playerSpeed = 3;

// Bullet variables
bool bulletActive = false;
int bulletX = 0;
int bulletY = 0;
const int bulletSpeed = 5;

// Enemy variables
const int maxEnemies = 5;
struct Enemy {
  int x;
  int y;
  bool active;
};
Enemy enemies[maxEnemies];

// Game variables
unsigned long lastEnemyMove = 0;
unsigned long enemyMoveInterval = 500;  // initial enemy speed (ms)
unsigned long lastEnemySpawn = 0;
unsigned long enemySpawnInterval = 1500;
int score = 0;
bool gameOver = false;

// MPU variables
int16_t ax, ay, az;
float accX, accY;
const float tiltThreshold = 10000;  // adjust sensitivity

void setup() {
  Serial.begin(9600);
  
  pinMode(FIRE_BUTTON_PIN, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Space Shooter!");
  display.display();
  delay(1500);

  Wire.begin();
  mpu.initialize();

  if(!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while(1);
  }

  // Initialize enemies inactive
  for (int i=0; i<maxEnemies; i++) {
    enemies[i].active = false;
  }
}

void spawnEnemy() {
  // Find inactive enemy slot
  for (int i=0; i<maxEnemies; i++) {
    if (!enemies[i].active) {
      enemies[i].active = true;
      enemies[i].x = random(5, SCREEN_WIDTH - 10);
      enemies[i].y = 0;
      break;
    }
  }
}

void moveEnemies() {
  unsigned long current = millis();
  if (current - lastEnemyMove > enemyMoveInterval) {
    lastEnemyMove = current;
    for (int i=0; i<maxEnemies; i++) {
      if (enemies[i].active) {
        enemies[i].y += 4; // Move down
        if (enemies[i].y > playerY) {
          gameOver = true;
        }
      }
    }
  }
}

void checkBulletCollision() {
  for (int i=0; i<maxEnemies; i++) {
    if (enemies[i].active && bulletActive) {
      // Simple collision check (circle overlap)
      if (abs(bulletX - enemies[i].x) < 6 && abs(bulletY - enemies[i].y) < 6) {
        enemies[i].active = false;
        bulletActive = false;
        score += 1;

        // Increase speed every 5 points
        if (score % 5 == 0 && enemyMoveInterval > 150) {
          enemyMoveInterval -= 50;
        }
      }
    }
  }
}

void readMPU() {
  mpu.getAcceleration(&ax, &ay, &az);
  accX = ax;
  accY = ay;
}

void handlePlayerMovement() {
  readMPU();
  // Use accX to move player left/right; MPU orientation may require tuning accX sign
  if (accX > tiltThreshold) {
    playerX += playerSpeed;
  } else if (accX < -tiltThreshold) {
    playerX -= playerSpeed;
  }

  // Clamp to screen
  if (playerX < 5) playerX = 5;
  if (playerX > SCREEN_WIDTH - 5) playerX = SCREEN_WIDTH - 5;
}

void handleBullet() {
  if (!bulletActive && digitalRead(FIRE_BUTTON_PIN) == LOW) {
    // Fire bullet from player position
    bulletActive = true;
    bulletX = playerX;
    bulletY = playerY - 5;
  }
  if (bulletActive) {
    bulletY -= bulletSpeed;
    if (bulletY < 0) {
      bulletActive = false;
    }
  }
}

void drawGame() {
  display.clearDisplay();

  // Draw player ship (triangle)
  display.fillTriangle(playerX, playerY,
                       playerX - 5, playerY + 7,
                       playerX + 5, playerY + 7,
                       SSD1306_WHITE);

  // Draw enemies (small filled circles)
  for (int i=0; i<maxEnemies; i++) {
    if (enemies[i].active) {
      display.fillCircle(enemies[i].x, enemies[i].y, 4, SSD1306_WHITE);
    }
  }

  // Draw bullet (small rectangle)
  if (bulletActive) {
    display.fillRect(bulletX-1, bulletY-3, 3, 6, SSD1306_WHITE);
  }

  // Draw score
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Score:");
  display.print(score);

  display.display();
}

void gameOverScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(20, 50);
  display.print("Score: ");
  display.print(score);
  display.display();
}

void loop() {
  if (!gameOver) {
    // Spawn enemies over time
    if (millis() - lastEnemySpawn > enemySpawnInterval) {
      spawnEnemy();
      lastEnemySpawn = millis();
    }

    handlePlayerMovement();
    handleBullet();
    moveEnemies();
    checkBulletCollision();
    drawGame();
  } else {
    gameOverScreen();
    delay(5000);
    // Reset game
    gameOver = false;
    score = 0;
    enemyMoveInterval = 500;
    for (int i=0; i<maxEnemies; i++) enemies[i].active = false;
    playerX = SCREEN_WIDTH/2;
  }
}

```

