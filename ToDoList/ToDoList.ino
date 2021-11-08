#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

#define MAX_CHARS 50

#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>

TinyScreen display = TinyScreen(0);


void setup(void) {
  Wire.begin();
  display.begin();
}

// 94x64 RGB Pixels

void loop() {
  int currX = 0;                // Current X pointer
  int currY = 0;                // Current Y pointer
  startScreen(&currX, &currY);  // Prints out default state
  int menuHeight = currY;       // Gets Y height of menu
  
  // Print out to do items
  printItem(&currX, &currY, "Walk the dog");
  printItem(&currX, &currY, "Eat lunch");
  printItem(&currX, &currY, "Do homework");

  // Button states, 1 => pressed, 0 => not pressed
  int LowerLeftState = 0;
  int UpperLeftState = 0;
  int LowerRightState = 0;
  int UpperRightState = 0;

  display.setCursor(currX, currY);
  while(1) {
    checkButtonStates(&LowerLeftState, &UpperLeftState, &LowerRightState, &UpperRightState);    
  }
}

void startScreen(int* currX, int* currY){
  // Display Main Menu.
  char* displayText = "To Do List";
  display.setFont(liberationSans_10ptFontInfo); 
  display.fontColor(WHITE,BLACK);
  int fontHeight = display.getFontHeight();
  int fontWidth = display.getPrintWidth(displayText);
  display.setBrightness(10);
  display.setFlip(true);
  display.print(displayText);
  display.drawLine(0, fontHeight, 94, fontHeight, RED);
  display.setFont(liberationSans_8ptFontInfo);

  // Change current X and Y values
  *currY = *currY + fontHeight; // Move Y by Font Height
  *currY = *currY + 1;          // Move Y by Line width
}

void printItem(int* currX, int* currY, char* item) {
  char newString[MAX_CHARS] = "> "; // Adds "-" before to do item
  strcat(newString, item);
  display.setCursor(*currX, *currY);
  display.print(newString);

  // Change current X and Y values
  *currY = *currY + display.getFontHeight(); // Move Y by Font Height
}

void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState) {
  // If Upper Left Button is pressed.
    if (display.getButtons(TSButtonUpperLeft)){
      *UpperLeftState = 1;
      if (UpperLeftState != 0) {
        delay(500); // Buffer for button press.
        display.print(*UpperLeftState); 
        *UpperLeftState = 0;
      }
    }

    // If Upper Right Button is pressed.
    if (display.getButtons(TSButtonUpperRight)){
      *UpperRightState = 1;
      if (*UpperRightState != 0) {
        delay(500); // Buffer for button press.
        display.print(*UpperRightState); 
        *UpperRightState = 0;
      }
    }

    // If Lower Left Button is pressed.
    if (display.getButtons(TSButtonLowerLeft)){
      *LowerLeftState = 1;
      if (*LowerLeftState != 0) {
        delay(500); // Buffer for button press.
        display.print(*LowerLeftState); 
        *LowerLeftState = 0;
      }
    }

    // If Lower Right Button is pressed.
    if (display.getButtons(TSButtonLowerRight)){
      *LowerRightState = 1;
      if (*LowerRightState != 0) {
        delay(500); // Buffer for button press.
        display.print(*LowerRightState); 
        *LowerRightState = 0;
      }
    }
}
  
  
  
