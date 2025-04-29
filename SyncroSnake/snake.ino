#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

// Define colors
#define BLACK 0x0000
#define GREEN 0x07E0
#define RED 0xF800

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// OLED pins
#define OLED_CS    10
#define OLED_DC    9
#define OLED_RST   8

// Initialize OLED display
Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC, OLED_RST);

// Snake properties
const int maxLength = 50; // Maximum snake length
int snakeX[maxLength] = {64}; // X positions of the snake
int snakeY[maxLength] = {64}; // Y positions of the snake
int snakeLength = 1; // Current length of the snake

// Joystick 1 and 2
int dirX = 0, dirY = 1; // Initial direction (down)

// Food position
int foodX, foodY;

// Joystick pins
const int VRx1 = A0; // X-axis of joystick 1
const int VRy1 = A1; // Y-axis of joystick 1
const int VRx2 = A2; // X-axis of joystick 2
const int VRy2 = A3; // Y-axis of joystick 2

void setup() {
  // Initialize the display
  display.begin();
  display.fillScreen(BLACK);

  // Display the start screen
  displayStartScreen();

  // Wait for joystick input to start the game
  waitForJoystickInput();

  // Generate the initial food position
  generateFood();
}

void loop() {
  // Read joystick 1 values
  int xValue1 = analogRead(VRx1);
  int yValue1 = analogRead(VRy1);

  // Read joystick 2 values
  int xValue2 = analogRead(VRx2);
  int yValue2 = analogRead(VRy2);

  // Determine direction for joystick 1
  int dirX1 = 0, dirY1 = 0;
  if (xValue1 < 400) dirX1 = -1; // Left
  else if (xValue1 > 600) dirX1 = 1; // Right
  else if (yValue1 < 400) dirY1 = -1; // Up
  else if (yValue1 > 600) dirY1 = 1; // Down

  // Determine direction for joystick 2
  int dirX2 = 0, dirY2 = 0;
  if (xValue2 < 400) dirX2 = -1; // Left
  else if (xValue2 > 600) dirX2 = 1; // Right
  else if (yValue2 < 400) dirY2 = -1; // Up
  else if (yValue2 > 600) dirY2 = 1; // Down

  // Only change direction if both joysticks agree on the same direction
  if (dirX1 == dirX2 && dirY1 == dirY2 && (dirX1 != 0 || dirY1 != 0)) {
    // Prevent the snake from moving in the opposite direction
    if (!(dirX1 == -dirX && dirY1 == -dirY)) {
      dirX = dirX1;
      dirY = dirY1;
    }
  }

  // Move the snake
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] += dirX * 8; // Move by 8 pixels
  snakeY[0] += dirY * 8;

  // Check for boundaries (wrap around)
  if (snakeX[0] < 0) snakeX[0] = SCREEN_WIDTH - 8;
  if (snakeX[0] >= SCREEN_WIDTH) snakeX[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = SCREEN_HEIGHT - 8;
  if (snakeY[0] >= SCREEN_HEIGHT) snakeY[0] = 0;

  // Check if the snake crashes into itself
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      resetGame(); // Restart the game
      return; // Exit the loop to avoid further processing
    }
  }

  // Check if the snake eats the food
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    addCube(); // Increase the snake's length
    generateFood(); // Generate a new food position
  }

  // Draw the snake
  display.fillScreen(BLACK); // Clear the screen
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i], snakeY[i], 8, 8, GREEN); // Draw each segment
  }

  // Draw the food
  display.fillRect(foodX, foodY, 8, 8, RED); // Draw the food

  delay(200); // Adjust speed
}

void addCube() {
  if (snakeLength < maxLength) {
    // Add a new segment at the same position as the last segment
    snakeX[snakeLength] = snakeX[snakeLength - 1];
    snakeY[snakeLength] = snakeY[snakeLength - 1];
    snakeLength++;
  }
}

void generateFood() {
  // Generate a random position for the food, aligned to the grid
  foodX = random(0, SCREEN_WIDTH / 8) * 8;
  foodY = random(0, SCREEN_HEIGHT / 8) * 8;
}

void resetGame() {
  // Display the start screen again
  displayStartScreen();

  // Wait for joystick input to restart the game
  waitForJoystickInput();

  // Reset snake properties
  snakeLength = 1;
  snakeX[0] = SCREEN_WIDTH / 2; // Start in the middle of the screen
  snakeY[0] = SCREEN_HEIGHT / 2;
  dirX = 0;
  dirY = 1;

  // Generate new food position
  generateFood();
}

void displayStartScreen() {
  display.setRotation(0); // Rotate the display by 180 degrees

  while (true) { // Flash the text continuously
    display.fillScreen(BLACK); // Clear the screen
    delay(300); // Pause for a moment

    display.setTextColor(GREEN); // Set text color
    display.setTextSize(2); // Set text size (big letters)

    // Display "Syncro" in the top row
    display.setCursor(20, 20); // Position for "Syncro"
    display.println("Syncro");

    // Display "Snake" below "Syncro"
    display.setCursor(30, 50); // Position for "Snake"
    display.println("Snake");

    delay(300); // Pause for a moment

    // Check for joystick input to exit the flashing loop
    int xValue1 = analogRead(VRx1); // Read X-axis of joystick 1
    int yValue1 = analogRead(VRy1); // Read Y-axis of joystick 1
    int xValue2 = analogRead(VRx2); // Read X-axis of joystick 2
    int yValue2 = analogRead(VRy2); // Read Y-axis of joystick 2

    if ((xValue1 < 400 || xValue1 > 600 || yValue1 < 400 || yValue1 > 600) ||
        (xValue2 < 400 || xValue2 > 600 || yValue2 < 400 || yValue2 > 600)) {
      break; // Exit the loop if either joystick is moved
    }
  }

  display.setRotation(90); // Reset rotation to default for the game
}

void waitForJoystickInput() {
  while (true) {
    int xValue1 = analogRead(VRx1); // Read X-axis of joystick 1
    int yValue1 = analogRead(VRy1); // Read Y-axis of joystick 1
    int xValue2 = analogRead(VRx2); // Read X-axis of joystick 2
    int yValue2 = analogRead(VRy2); // Read Y-axis of joystick 2

    // Check if there is joystick input from either joystick
    if ((xValue1 < 400 || xValue1 > 600 || yValue1 < 400 || yValue1 > 600) ||
        (xValue2 < 400 || xValue2 > 600 || yValue2 < 400 || yValue2 > 600)) {
      break; // Exit the loop if either joystick is moved
    }
  }
}