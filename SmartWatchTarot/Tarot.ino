/*
#define BLACK           0x00
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
int currX = 0;                    // Current X pointer
int currY = 0;
bool dataSent = false;
long int rando = 0;

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


 
  // Get the acceleration values from the sensor and store them into global variables
  // (Makes reading the rest of the program easier)
  

  // If the BMA250 is not found, nor connected correctly, these values will be produced
  // by the sensor 
  
   // if we have correct sensor readings: 
    showCard();                 //Print to Serial Monitor or Plotter
    
  // The BMA250 can only poll new sensor values every 64ms, so this delay
  // will ensure that we can continue to read values
  delay(25);
  // ***Without the delay, there would not be any sensor output*** 
  

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
      checkButtonStates(&LowerLeftState, &UpperLeftState, &LowerRightState, &UpperRightState,&currX,&currY);  // Check for button inputs.
      checkBluetooth(bufferString); // Check for bluetooth inputs.
      if (dataSent){
        // If bluetooth input received, create new task on watch.
        printItem(&currX, &currY, bufferString);
        dataSent = false;
      }
      
    }
    
     
  }



// Prints the sensor values to the Serial Monitor, or Serial Plotter (found under 'Tools')
void showCard() {
  

  rando = random(1,78); //You need a random range 1 more than the number of statements you have
  rando = round(rando);
  
  
  if (rando == 1){
    showClear(currX, currY);
    printItem(&currX,&currY,"Two of Cups");
    

    }
    if (rando == 2){
    showClear(currX, currY);
    printItem(&currX,&currY,"The High Priestess");
    
    }
    if (rando == 3){
    showClear(currX, currY);
    printItem(&currX,&currY,"Four of Cups");
    }
    if (rando == 4){
    showClear(currX, currY);
    printItem(&currX,&currY,"Five of Cups");
    }
    if (rando == 5){
    showClear(currX, currY);
    printItem(&currX,&currY,"Six of Cups");
    
    }
    if (rando == 6){
    showClear(currX, currY);
    printItem(&currX,&currY,"Seven of Cups");
    
    }
    if (rando == 7){
    showClear(currX, currY);
    printItem(&currX,&currY,"Eight of Cups");
    
    }
    if (rando == 8){
    showClear(currX, currY);
    printItem(&currX,&currY,"Nine of Cups");
    
    
    }
    if (rando == 9){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ten of Cups");
    
    }
    if (rando == 10){
    showClear(currX, currY);
    printItem(&currX,&currY,"Page of Cups");
    
    }
    if (rando == 11){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ace of Cups");
    
    }
    if (rando == 12){
    showClear(currX, currY);
    printItem(&currX,&currY,"Knight of Cups");
    
    }
    if (rando == 13){
    showClear(currX, currY);
    printItem(&currX,&currY,"Queen of Cups");
    }
    if (rando == 14){
    showClear(currX, currY);
    printItem(&currX,&currY,"King of Cups");
    }
    if (rando == 15){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Fool");
    }
    if (rando == 16){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Magician");
    }
    if (rando == 17){
    showClear(currX, currY);
    printItem(&currX,&currY,"The High Priestess");
    }
    if (rando == 18){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Empress");
    }
    if (rando == 19){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Emperor");
    }
    if (rando == 20){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Hierophant");
    }
    if (rando == 21){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Lovers");
    }
    if (rando == 22){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Chariot");
    }
    if (rando == 23){
    showClear(currX, currY);
    printItem(&currX,&currY,"Strength");
    }
    if (rando == 24){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Hermit");
    }
    if (rando == 25){
    showClear(currX, currY);
    printItem(&currX,&currY,"Wheel of Fortune");
    }
    if (rando == 26){
    showClear(currX, currY);
    printItem(&currX,&currY,"Justice");
    }
    if (rando == 27){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Hanged Man");
    }
    if (rando == 28){
    showClear(currX, currY);
    printItem(&currX,&currY,"Death");
    }
    if (rando == 29){
    showClear(currX, currY);
    printItem(&currX,&currY,"Temperance");
    }
    if (rando == 30){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Devil");
    }
    if (rando == 31){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Tower");
    }
    if (rando == 32){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Star");
    }
    if (rando == 33){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Moon");
    }
    if (rando == 34){
    showClear(currX, currY);
    printItem(&currX,&currY,"The Sun");
    }
    if (rando == 35){
    showClear(currX, currY);
    printItem(&currX,&currY,"Judgement");
    }
    if (rando == 36){
    showClear(currX, currY);
    printItem(&currX,&currY,"The World");
    }
    if (rando == 37){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ace of Wands");
    }
    if (rando == 38){
    showClear(currX, currY);
    printItem(&currX,&currY,"Two of Wands");
    }
    if (rando == 39){
    showClear(currX, currY);
    printItem(&currX,&currY,"Three of Wands");
    }
    if (rando == 40){
    showClear(currX, currY);
    printItem(&currX,&currY,"Four of Wands");
    }
    if (rando == 41){
    showClear(currX, currY);
    printItem(&currX,&currY,"Five of Wands");
    }
    if (rando == 42){
    showClear(currX, currY);
    printItem(&currX,&currY,"Six of Wands");
    }
    if (rando == 43){
    showClear(currX, currY);
    printItem(&currX,&currY,"Seven of Wands");
    }
    if (rando == 44){
    showClear(currX, currY);
    printItem(&currX,&currY,"Eight of Wands");
    }
    if (rando == 45){
    showClear(currX, currY);
    printItem(&currX,&currY,"Nine of Wands");
    }
    if (rando == 46){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ten of Wands");
    }
    if (rando == 47){
    showClear(currX, currY);
    printItem(&currX,&currY,"Page of Wands");
    }
    if (rando == 48){
    showClear(currX, currY);
    printItem(&currX,&currY,"Knight of Wands");
    }
    if (rando == 49){
    showClear(currX, currY);
    printItem(&currX,&currY,"Queen of Wands");
    }
    if (rando == 50){
    showClear(currX, currY);
    printItem(&currX,&currY,"King of Wands");
    }
    if (rando == 51){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ace of Swords");
    }
    if (rando == 52){
    showClear(currX, currY);
    printItem(&currX,&currY,"Two of Swords");
    }
    if (rando == 53){
    showClear(currX, currY);
    printItem(&currX,&currY,"Three of Swords");
    }
    if (rando == 54){
    showClear(currX, currY);
    printItem(&currX,&currY,"Four of Swords");
    }
    if (rando == 55){
    showClear(currX, currY);
    printItem(&currX,&currY,"Five of Swords");
    }
    if (rando == 56){
    showClear(currX, currY);
    printItem(&currX,&currY,"Six of Swords");
    }
    if (rando == 57){
    showClear(currX, currY);
    printItem(&currX,&currY,"Seven of Swords");
    }
    if (rando == 58){
    showClear(currX, currY);
    printItem(&currX,&currY,"Eight of Swords");
    }
    if (rando == 59){
    showClear(currX, currY);
    printItem(&currX,&currY,"Nine of Swords");
    }
    if (rando == 60){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ten of Swords");
    }
    if (rando == 61){
    showClear(currX, currY);
    printItem(&currX,&currY,"Page of Swords");
    }
    if (rando == 62){
    showClear(currX, currY);
    printItem(&currX,&currY,"Knight of Swords");
    }
    if (rando == 63){
    showClear(currX, currY);
    printItem(&currX,&currY,"Queen of Swords");
    }
    if (rando == 64){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ace of Pentacles");
    }
    if (rando == 65){
    showClear(currX, currY);
    printItem(&currX,&currY,"Two of Pentacles");
    }
    if (rando == 66){
    showClear(currX, currY);
    printItem(&currX,&currY,"Three of Pentacles");
    }
    if (rando == 67){
    showClear(currX, currY);
    printItem(&currX,&currY,"Four of Pentacles");
    }
    if (rando == 68){
    showClear(currX, currY);
    printItem(&currX,&currY,"Five of Pentacles");
    }
    if (rando == 69){
    showClear(currX, currY);
    printItem(&currX,&currY,"Six of Pentacles");
    }
    if (rando == 70){
    showClear(currX, currY);
    printItem(&currX,&currY,"Seven of Pentacles");
    }
    if (rando == 71){
    showClear(currX, currY);
    printItem(&currX,&currY,"Eight of Pentacles");
    }
    if (rando == 72){
    showClear(currX, currY);
    printItem(&currX,&currY,"Nine of Pentacles");
    }
    if (rando == 73){
    showClear(currX, currY);
    printItem(&currX,&currY,"Ten of Pentacles");
    }
    if (rando == 74){
    showClear(currX, currY);
    printItem(&currX,&currY,"Page of Pentacles");
    }
    if (rando == 75){
    showClear(currX, currY);
    printItem(&currX,&currY,"Knight of Pentacles");
    }
    if (rando == 76){
    showClear(currX, currY);
    printItem(&currX,&currY,"Queen of Pentacles");
    }
    if (rando == 77){
    showClear(currX, currY);
    printItem(&currX,&currY,"King of Pentacles");
    }

    
    
  
  
  
  
  
  
 
}
void showClear(int currX, int currY){
  display.clearScreen(); 
  currX = 0;
  currY = 0;
  startScreen(&currX, &currY);
  
  
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
  display.setCursor(*currX, *currY);
  char* displayText = "Tarot Reading";
  display.setFont(liberationSans_10ptFontInfo); 
  display.fontColor(WHITE,BLACK);
  int fontHeight = display.getFontHeight();
  int fontWidth = display.getPrintWidth(displayText);
  display.setBrightness(10);
  display.setFlip(true);
  display.print(displayText);
  
  //display.drawLine(0, fontHeight, 94, fontHeight, RED);
  display.setFont(liberationSans_8ptFontInfo);

  // Change current X and Y values
  *currY = *currY + fontHeight; // Move Y by Font Height
  *currY = *currY + 1;          // Move Y by Line width
}




//------------------------------------------------------
// Add Item to To Do List
//------------------------------------------------------
void printItem(int* currX, int* currY, char* item) {
  char newString[MAX_CHARS] = ""; // Adds "-" before to do item
  strcat(newString, item);
  display.setCursor(*currX, *currY);
  display.print(newString);

  //Change current X and Y values
  //*currY = *currY + display.getFontHeight(); // Move Y by Font Height
}





//------------------------------------------------------
// Check for Button Presses
//------------------------------------------------------
void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState,int* currX, int* currY) {
  // If Upper Left Button is pressed.
    if (display.getButtons(TSButtonUpperLeft)){
      *UpperLeftState = 1;
      if (UpperLeftState != 0) {
        delay(500); // Buffer for button press.
         showCard(); 
        *UpperLeftState = 0;
      }
    }

    // If Upper Right Button is pressed.
    if (display.getButtons(TSButtonUpperRight)){
      *UpperRightState = 1;
      if (*UpperRightState != 0) {
       
        delay(500); // Buffer for button press.
        //showClear(0,0);
        display.clearScreen(); 
        *currX = 0;
        *currY = 0;
        
        // Reprint the screen, with new item
        startScreen(currX, currY);
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
*/
