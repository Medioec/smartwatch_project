#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

#define MAX_CHARS 20 // Max Characters

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

//------------------------------------------------------
// Main Loop
//------------------------------------------------------
void loop() {
  int currX = 0;                    // Current X pointer
  int currY = 0;                    // Current Y pointer
  startScreen(&currX, &currY);      // Prints out default state
  int menuHeight = currY;           // Gets Y height of menu
  char* bufferString = (char*)malloc(MAX_CHARS * sizeof(char));
  
  
  // Button states, 1 => pressed, 0 => not pressed
  int LowerLeftState = 0;
  int UpperLeftState = 0;
  int LowerRightState = 0;
  int UpperRightState = 0;
  
  display.setCursor(currX, currY);
  while(1) {
    checkButtonStates(&LowerLeftState, &UpperLeftState, &LowerRightState, &UpperRightState);  // Check for button inputs.
    checkBluetooth(bufferString); // Check for bluetooth inputs.
    if (dataSent){
      // If bluetooth input received, create new task on watch.
      printItem(&currX, &currY, bufferString);
      dataSent = false;
    }
    
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
// Print Main Menu
//------------------------------------------------------
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

//------------------------------------------------------
// Add Item to To Do List
//------------------------------------------------------
void printItem(int* currX, int* currY, char* item) {
  char newString[MAX_CHARS] = "> "; // Adds "-" before to do item
  strcat(newString, item);
  display.setCursor(*currX, *currY);
  display.print(newString);

  // Change current X and Y values
  *currY = *currY + display.getFontHeight(); // Move Y by Font Height
}

//------------------------------------------------------
// Check for Button Presses
//------------------------------------------------------
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
  
  
  
