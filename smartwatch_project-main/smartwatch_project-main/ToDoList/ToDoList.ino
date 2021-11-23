#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32
#define SELECTED        0x80

#define MAX_CHARS 20 // Max Characters

#define MAX_ITEMS 4 // Max  To Do List Items

#include <TinyScreen.h>
#include <SPI.h>
#include <Wire.h>

//------------------------------------------------------
// Setup Bluetooth Configuration
//------------------------------------------------------
#include <STBLE.h>
//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif


uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

bool dataSent = false;
TinyScreen display = TinyScreen(0);

void addItem(char bufferArray[][MAX_CHARS + 1], char* item, int* currCount, int* ErrorState);
void printDisplay(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currSize, int* currX, int* currY);
void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState, int* selectedIndex, int* currCount, int* currX, int* currY, char bufferArray[MAX_ITEMS][MAX_CHARS + 1]);
void error1(int* currX, int* currY, int* ErrorState);

//------------------------------------------------------
// Setup TinyScreen
//------------------------------------------------------
void setup(void) {
  // Bluetooth configuration
  SerialMonitorInterface.begin(9600);
//  while (!SerialMonitorInterface); //This line will block until a serial monitor is opened with TinyScreen+!
  BLEsetup();
  
  Wire.begin();
  display.begin();
}

// 94x64 RGB Pixels

// Button UpperLeft -> Up
// Button LowerLeft -> Down
// Button LowerRight -> Trigger On and Off
// Button UpperRight -> Delete

// Store all the to do list items in an array
// If user selects, print that selected item as font color SELECTED, if not White
// If user deletes, remove from array
// If array length more than 4, stores at next page.

//------------------------------------------------------
// Main Loop
//------------------------------------------------------
void loop() {
  int currX = 0;                      // Current X pointer
  int currY = 0;                      // Current Y pointer
  
  char* bufferString = (char*)malloc(MAX_CHARS * sizeof(char));

  char bufferArray[MAX_ITEMS][MAX_CHARS + 1]; // Array to store all items
  int selectedIndex = 0;                      // Index that is currently selected.
  int currCount = 0;                          // Current count of array
  
  // Button states, 1 => pressed, 0 => not pressed
  int LowerLeftState = 0;
  int UpperLeftState = 0;
  int LowerRightState = 0;
  int UpperRightState = 0;

  // Error state, 0 => no error, 1 => too many items
  int ErrorState = 0;
  
  startScreen(&currX, &currY);        // Prints out default state
  
  addItem(bufferArray, "Walk the Dog", &currCount, &ErrorState);
  addItem(bufferArray, "Walk the Cat", &currCount, &ErrorState);
  addItem(bufferArray, "Walk the Husky", &currCount, &ErrorState);
  printDisplay(bufferArray, &selectedIndex, &currCount, &currX, &currY);

  display.setBrightness(10);
  display.setFlip(true);
  display.setCursor(currX, currY);
  
  while(1) {
    checkButtonStates(&LowerLeftState, &UpperLeftState, &LowerRightState, &UpperRightState, &selectedIndex, &currCount, &currX, &currY, bufferArray);  // Check for button inputs.
    checkBluetooth(bufferString); // Check for bluetooth inputs, if avail, send to buffer string

    // Handle bluetooth input
    if (dataSent){
      // If bluetooth input received, create new task on watch.
      addItem(bufferArray, bufferString, &currCount, &ErrorState);
      dataSent = false;

      // Handle too many items error
      if(ErrorState == 1) {
        error1(&currX, &currY, &ErrorState);
        ErrorState = 0;
      }
      
      // Reprint the screen, with new item
      
      // Clear screen, reset X and Y pointers
      display.clearScreen(); 
      currX = 0;
      currY = 0;
      
      // Print current bufferarray
      startScreen(&currX, &currY);
      printDisplay(bufferArray, &selectedIndex, &currCount, &currX, &currY);
    }
  }
}

//------------------------------------------------------
// Print Too Many Items Error, (error 1)
//------------------------------------------------------
void error1(int* currX, int* currY, int* ErrorState) {
  int tmpX = 0;
  int tmpY = 0;

  char* errorText = "ERROR!!!";
  char* errorText2 = "More than 4 items detected";
  
  display.clearScreen();
  display.setCursor(tmpX, tmpY);
  display.setFont(liberationSans_10ptFontInfo); 
  display.fontColor(RED,BLACK);

  display.print(errorText);
  int fontHeight = display.getFontHeight();
  tmpY += fontHeight;
  display.setCursor(tmpX, tmpY);
  display.print(errorText2);

  delay(2000); // Error display length
  display.setCursor(*currX, *currY); // Move cursor back to original location
}


//------------------------------------------------------
// Print Main Menu
//------------------------------------------------------
void startScreen(int* currX, int* currY){
  // Display Main Menu.

  display.setCursor(*currX, *currY);
  
  char* displayText = "To Do List";
  display.setFont(liberationSans_10ptFontInfo); 
  display.fontColor(WHITE,BLACK);
  
  int fontHeight = display.getFontHeight();
  int fontWidth = display.getPrintWidth(displayText);
  
  display.setFlip(true);
  display.print(displayText);
  display.drawLine(0, fontHeight, 94, fontHeight, RED);
  display.setFont(liberationSans_8ptFontInfo);

  // Change current X and Y values
  *currY = *currY + fontHeight; // Move Y by Font Height
  *currY = *currY + 1;          // Move Y by Line width
}

//------------------------------------------------------
// Print display on watch
//------------------------------------------------------
void printDisplay(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currSize, int* currX, int* currY) {
    for (int i = 0; i < *currSize; i++){
        display.setCursor(*currX, *currY);
        char* currString = *(bufferArray + i); // Current string iteration
        if (i == *selectedIndex){
        // If current printed is selected
        display.fontColor(SELECTED, BLACK);
        display.print(currString);
      } else {
        display.fontColor(WHITE, BLACK);
        display.print(currString);
      }
      *currY = *currY + display.getFontHeight(); // Move Y by Font Height
    }
}

//------------------------------------------------------
// Adds Item to Array
//------------------------------------------------------
void addItem(char bufferArray[][MAX_CHARS + 1], char* item, int* currCount, int* ErrorState) {
  if (*currCount < MAX_ITEMS){
    char newString[MAX_CHARS + 1] = "> "; // Adds "-" before to do item
    strcat(newString, item);
    strcpy(*(bufferArray + *currCount), newString); // Add string to array
    *currCount += 1;                                // Add to current index
  } else {
    *ErrorState = 1; // Set too many items error
  }
}

//------------------------------------------------------
// Removes Item from Array
//------------------------------------------------------
void removeItem(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currCount) {
  // Remove select item index from array.
  if (*currCount > 0){
    bool deleted = false;
    for (int i = 0; i < MAX_ITEMS; i++) {
      
      if(i == *selectedIndex) {
        deleted = true;
      }
      if(deleted) {
        // If index in range, shift next index in
        if(i + 1 <= MAX_ITEMS - 1){
          strcpy(bufferArray[i],bufferArray[i+1]);
        } 
      }
    }
  
    *selectedIndex = 0;
    *currCount -= 1;
  }
}

//------------------------------------------------------
// Check for Bluetooth Input, if avail, save to bufferstring
//------------------------------------------------------
void checkBluetooth (char* myBufferString){
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {//Check if data is available
    strcpy(myBufferString, "");
    SerialMonitorInterface.print(ble_rx_buffer_len);
    SerialMonitorInterface.print(" : ");
    SerialMonitorInterface.println((char*)ble_rx_buffer);
    strcpy(myBufferString, (char*)ble_rx_buffer);
    dataSent = true;
    ble_rx_buffer_len = 0;//clear afer reading
  }
  if (SerialMonitorInterface.available()) {//Check if serial input is available to send
    delay(10);//should catch input
    uint8_t sendBuffer[21];
    uint8_t sendLength = 0;
    while (SerialMonitorInterface.available() && sendLength < 19) {
      sendBuffer[sendLength] = SerialMonitorInterface.read();
      sendLength++;
    }
    if (SerialMonitorInterface.available()) {
      SerialMonitorInterface.print(F("Input truncated, dropped: "));
      if (SerialMonitorInterface.available()) {
        SerialMonitorInterface.write(SerialMonitorInterface.read());
      }
    }
    sendBuffer[sendLength] = '\0'; //Terminate string
    sendLength++;
    if (!lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength))
    {
      SerialMonitorInterface.println(F("TX dropped!"));
    }
  }
}

//------------------------------------------------------
// Check for Button Presses, if found execute
//------------------------------------------------------
void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState, int* selectedIndex, int* currCount, int* currX, int* currY, char bufferArray[MAX_ITEMS][MAX_CHARS + 1]) {
  // If Upper Left Button is pressed.
    if (display.getButtons(TSButtonUpperLeft)){
      *UpperLeftState = 1;
      if (UpperLeftState != 0) {
        delay(500); // Buffer for button press.

        // Select Previous
        *selectedIndex -= 1;
        if(*selectedIndex < 0){
          // If out of bounds, go back to first index.
          *selectedIndex = 0;
        }

        // Clear screen, reset X and Y pointers
        display.clearScreen(); 
        *currX = 0;
        *currY = 0;
        
        // Reprint the screen, with new item
        startScreen(currX, currY);
        printDisplay(bufferArray, selectedIndex, currCount, currX, currY);


        *UpperLeftState = 0;
      }
    }

    // If Upper Right Button is pressed.
    if (display.getButtons(TSButtonUpperRight)){
      *UpperRightState = 1;
      if (*UpperRightState != 0) {
        delay(500); // Buffer for button press.

        // Remove items
        removeItem(bufferArray, selectedIndex, currCount);

        // Clear screen, reset X and Y pointers
        display.clearScreen(); 
        *currX = 0;
        *currY = 0;
        
        // Reprint the screen, with new item
        startScreen(currX, currY);
        printDisplay(bufferArray, selectedIndex, currCount, currX, currY);
        *UpperRightState = 0;
      }
    }

    // If Lower Left Button is pressed.
    if (display.getButtons(TSButtonLowerLeft)){
      *LowerLeftState = 1;
      if (*LowerLeftState != 0) {
        delay(500); // Buffer for button press.

        // Select Next To Do List
        *selectedIndex += 1;
        if (*selectedIndex > *currCount - 1) {
          // If out of bounds, go back to first index.
          *selectedIndex = 0;
        }

        // Clear screen, reset X and Y pointers
        display.clearScreen(); 
        *currX = 0;
        *currY = 0;
        
        // Reprint the screen, with new item
        startScreen(currX, currY);
        printDisplay(bufferArray, selectedIndex, currCount, currX, currY);

        *LowerLeftState = 0;
      }
    }

    // If Lower Right Button is pressed.
    if (display.getButtons(TSButtonLowerRight)){
      *LowerRightState = 1;
      if (*LowerRightState != 0) {
        delay(500); // Buffer for button press.
//        display.print(*LowerRightState); 
        *LowerRightState = 0;
      }
    }
}
  
  
  
