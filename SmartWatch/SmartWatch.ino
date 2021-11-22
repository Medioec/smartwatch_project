//-------------------------------------------------------------------------------
//  TinyCircuits TinyScreen/ST BLE ANCS Smartwatch Example
//  Last Updated 26 October 2017
//
//  This demo sets up the ST BLE for the Apple Notification Center Service as well
//  as the Current Time service. Now with TinyScreen+ compatibity.
//  Requires updated STBLE library or will not compile!
//
//  2.0.0 26 Oct 2017 Initial update release
//
//  Written by Ben Rose, TinyCircuits http://TinyCircuits.com
//
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <Wire.h>
#include <TinyScreen.h>
#include <STBLE.h>
#include <LibPrintf.h>
#include "BMA250.h" 

#define BLE_DEBUG true
#define menu_debug_print true
uint32_t doVibrate = 0;

//#define ARDUINO_ARCH_SAMD

#if defined (ARDUINO_ARCH_AVR)
TinyScreen display = TinyScreen(TinyScreenDefault);
#define SerialMonitorInterface Serial
#include <TimeLib.h>

#elif defined(ARDUINO_ARCH_SAMD)
TinyScreen display = TinyScreen(TinyScreenDefault);
#define SerialMonitorInterface SerialUSB
#include <RTCZero.h>
#include <time.h>
RTCZero RTCZ;
uint32_t startTime = 0;
uint32_t sleepTime = 0;
unsigned long millisOffsetCount = 0;

void wakeHandler() {
  if (sleepTime) {
    millisOffsetCount += (RTCZ.getEpoch() - sleepTime);
    sleepTime = 0;
  }
}

void RTCwakeHandler() {
  //not used
}
uint8_t ble_can_sleep = false;
void watchSleep() {
  if (doVibrate || ble_can_sleep)
    return;
  sleepTime = RTCZ.getEpoch();
  RTCZ.standbyMode();
}
#endif
/*
BLEConn phoneConnection;
BLEServ timeService;
BLEServ ANCSService;
BLEChar currentTimeChar;
BLEChar NSchar;
BLEChar CPchar;
BLEChar DSchar;
int ANCSInitStep = -1;
unsigned long ANCSInitRetry = 0;*/

uint8_t ble_connection_state = false;
uint8_t ble_connection_displayed_state = true;
uint8_t TimeData[20];
uint32_t newtime = 0;

uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;

uint8_t defaultFontColor = TS_8b_White;
uint8_t defaultFontBG = TS_8b_Black;
uint8_t inactiveFontColor = TS_8b_Gray;
uint8_t inactiveFontBG = TS_8b_Black;

uint8_t topBarHeight = 10;
uint8_t timeY = 14;
uint8_t menuTextY[4] = {12, 25, 38, 51};

unsigned long lastReceivedTime = 0;

unsigned long batteryUpdateInterval = 10000;
unsigned long lastBatteryUpdate = 0;

unsigned long sleepTimer = 0;
int sleepTimeout = 0;

uint8_t rewriteTime = true;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
uint8_t amtNotifications = 0;
uint8_t lastAmtNotificationsShown = -1;
unsigned long mainDisplayUpdateInterval = 16;
unsigned long lastMainDisplayUpdate = 0;
char notificationLine1[20] = "";
char notificationLine2[20] = "";

uint8_t vibratePin = 6;
uint8_t vibratePinActive = HIGH;
uint8_t vibratePinInactive = LOW;


int brightness = 3;
uint8_t lastSetBrightness = 100;

const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font8pt = liberationSansNarrow_8ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;

//moved from menu
const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;
const uint8_t displayStateTimer = 0x04;
const uint8_t displayPSIM = 0x05;

uint8_t currentDisplayState = displayStateHome;
void (*editIntCallBack)() = NULL;

const uint8_t upButton = TSButtonUpperRight;
const uint8_t downButton = TSButtonLowerRight;
const uint8_t selectButton = TSButtonLowerLeft;
const uint8_t backButton = TSButtonUpperLeft;
const uint8_t menuButton = TSButtonLowerLeft;
const uint8_t viewButton = TSButtonLowerRight;
const uint8_t clearButton = TSButtonLowerRight;

//Timer Variables
int userTimerSetting = 0;
int userTimerSetState = 0;
int userTimerRunningState = 0;
int userTimerLastValue = 0;
int userTimerCurrentValue = 0;
int userTimerStartTime = 0;

//PSIM Variables
int psVarInitLaunch = 1;
int psVarspeed = 50;
int psVarContinueAllowed = 0;
int psimRunState = 0;
uint32_t psRelTime = 0;

//PVAR
int psVarScore = 0;
int psVarSpreadability = 0;
int psVarSeverity = 0;
int psVarhotAug = 0;
int psVarColdAug = 0;

//AVAR
float psVarNumTotal = 7900000000;
float psVarNumSpread = 1;
int psVarSpreadResist = 0;
int psVarCounterProgress = 0;
int psVarSrUpProb = 0;
int psVarCpUpProb = 0;

uint8_t (*psimHandler)(uint8_t) = NULL;

//fefewfewfewfewfewfewfewfewfewfewfewfewfewfe  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
BMA250 accel_sensor;
int x, y, z;
double temp;
//fefewfewfewfewfewfwefwfewfefewfewfewfewfewfewf  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

void setup(void)
{
#if defined (ARDUINO_ARCH_AVR)
  for (int i = 0; i < 20; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  setTime(1, 1, 1, 16, 2, 2016);
#elif defined(ARDUINO_ARCH_SAMD)
  /*for (int i = 0; i < 27; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(45, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(2, INPUT);*/
  RTCZ.begin();
  RTCZ.setTime(16, 15, 1);//h,m,s
  RTCZ.setDate(25, 7, 16);//d,m,y
  //RTCZ.attachInterrupt(RTCwakeHandler);
  //RTCZ.enableAlarm(RTCZ.MATCH_HHMMSS);
  //RTCZ.setAlarmEpoch(RTCZ.getEpoch() + 1);
  //attachInterrupt(TSP_PIN_BT1, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT2, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT3, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT4, wakeHandler, FALLING);
#endif
  Wire.begin();
  SerialMonitorInterface.begin(115200);
  //while(!SerialMonitorInterface);
  //dwqddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
  SerialMonitorInterface.print("Initializing BMA...");
  // Set up the BMA250 acccelerometer sensor
  accel_sensor.begin(BMA250_range_2g, BMA250_update_time_64ms); 
  //dwqdqwdqwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

  display.begin();
  display.setFlip(true);
  pinMode(vibratePin, OUTPUT);
  digitalWrite(vibratePin, vibratePinInactive);
  initHomeScreen();
  requestScreenOn();
  delay(100);
  BLEsetup();
  //useSecurity(BLEBond);
  //advertise("TinyWatch", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");

#if defined(ARDUINO_ARCH_SAMD)
  //attachInterrupt(TSP_PIN_BT1, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT2, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT3, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT4, wakeHandler, FALLING);
  // https://github.com/arduino/ArduinoCore-samd/issues/142
  // Clock EIC in sleep mode so that we can use pin change interrupts
  // The RTCZero library will setup generic clock 2 to XOSC32K/32
  // and we'll use that for the EIC. Increases power consumption by ~50uA
  //GCLK->CLKCTRL.reg = uint16_t(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID( GCLK_CLKCTRL_ID_EIC_Val ) );
  //while (GCLK->STATUS.bit.SYNCBUSY) {}
#endif
  //while (millisOffset() < 1000)BLEProcess();
  //display.off();
  //while (1) RTCZ.standbyMode();
}

uint32_t millisOffset() {
#if defined (ARDUINO_ARCH_AVR)
  return millis();
#elif defined(ARDUINO_ARCH_SAMD)
  return (millisOffsetCount * 1000ul) + millis();
#endif
}

void loop() {
//ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
 accel_sensor.read();//This function gets new data from the acccelerometer
  // Get the acceleration values from the sensor and store them into global variables
  // (Makes reading the rest of the program easier)
  temp = ((accel_sensor.rawTemp * 0.5) + 24.0);

  // If the BMA250 is not found, nor connected correctly, these values will be produced
  // by the sensor 
  
   // if we have correct sensor readings: 
                 //Print to Serial Monitor or Plotter
    
  // The BMA250 can only poll new sensor values every 64ms, so this delay
  // will ensure that we can continue to read values
  delay(25);
  // ***Without the delay, there would not be any sensor output*** 
//dwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwdddddddddddddddddddddddddddddddddddddddddd  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

  /*BLEProcess();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (!ANCSInitStep) {
    ANCSInit();
  } else if (ANCSInitRetry && millisOffset() - ANCSInitRetry > 1000) {
    ANCSInit();
  }
  ANCSProcess();

  if (ANCSIsBusy()) {
    return;
  }

  amtNotifications = ANCSNotificationCount();

  if (newtime) {
    newtime = 0;
    newTimeData();
  }

  if (ANCSNewNotification()) {
    requestScreenOn();
    rewriteMenu = true;
    updateMainDisplay();
    doVibrate = millisOffset();
  }*/
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {
    if (ble_rx_buffer[0] == 'D') {
      //expect date/time string- example: D2015 03 05 11 48 42
      lastReceivedTime = millisOffset();
      updateTime(ble_rx_buffer + 1);
      requestScreenOn();
    }
    if (ble_rx_buffer[0] == '1') {
      memcpy(notificationLine1, ble_rx_buffer + 1, ble_rx_buffer_len - 1);
      notificationLine1[ble_rx_buffer_len - 1] = '\0';
      amtNotifications = 1;
      requestScreenOn();
    }
    if (ble_rx_buffer[0] == '2') {
      memcpy(notificationLine2, ble_rx_buffer + 1, ble_rx_buffer_len - 1);
      notificationLine2[ble_rx_buffer_len - 1] = '\0';
      amtNotifications = 1;
      requestScreenOn();
      rewriteMenu = true;
      updateMainDisplay();
      doVibrate = millisOffset();
    }
    ble_rx_buffer_len = 0;
  }

  if (doVibrate) {
    uint32_t td = millisOffset() - doVibrate;
    if (td > 0 && td < 100) {
      digitalWrite(vibratePin, vibratePinActive);
    } else if (td > 200 && td < 300) {
      digitalWrite(vibratePin, vibratePinActive);
    } else {
      digitalWrite(vibratePin, vibratePinInactive);
      if (td > 300)doVibrate = 0;
    }
  }
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    updateMainDisplay();
  }
  if (millisOffset() > sleepTimer + ((unsigned long)sleepTimeout * 1000ul) && sleepTimeout != 0) {
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
#if defined(ARDUINO_ARCH_SAMD)
    //BLEProcess();
    //watchSleep();
#endif
  }
  checkButtons();
  if (psimRunState) {
    tickGame();
  }
}

void updateTime(uint8_t * b) {
  int y, M, d, k, m, s;
  char * next;
  y = strtol((char *)b, &next, 10);
  M = strtol(next, &next, 10);
  d = strtol(next, &next, 10);
  k = strtol(next, &next, 10);
  m = strtol(next, &next, 10);
  s = strtol(next, &next, 10);
#if defined (ARDUINO_ARCH_AVR)
  setTime(k, m, s, d, M, y);
#elif defined(ARDUINO_ARCH_SAMD)
  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
#endif
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    displayOn = 1;
    updateMainDisplay();
    display.on();
    return 1;
  }
  return 0;
}

//called in main loop to check for buttons
void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn)
      buttonPress(buttons);
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x0F)) {
    buttonReleased = 1;
  }
}

/*
void newTimeData() {
  int y, M, d, k, m, s;
  y = (TimeData[1] << 8) | TimeData[0];
  M = TimeData[2];
  d = TimeData[3];
  k = TimeData[4];
  m = TimeData[5];
  s = TimeData[6];
  //dayOfWeek = timeData[7];
  //fractionOfSecond = timeData[8];
#if defined (ARDUINO_ARCH_AVR)
  setTime(k, m, s, d, M, y);
#elif defined(ARDUINO_ARCH_SAMD)
  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
#endif
}*/

//dwqdqwdqwdqwdqwdqwdqwdwdwddddddddddddddddddddddddddddddddddddddddddddddddddd  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
void showSerial() {
   
  display.setCursor(10, menuTextY[1]);
  display.print("Today's Temp(C):");
    display.setCursor(30, menuTextY[3]);

  display.print(temp);
    display.setCursor(30, menuTextY[3]);
 
}
//djhhhhhhhhhhhhhhhhhhhhhhhddddddddddddddddddddddddddddddddddddddddddddddddddd  //change the valueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

/*
void timeCharUpdate(uint8_t * newData, uint8_t length) {
  memcpy(TimeData, newData, length);
  newtime = millisOffset();
  //SerialMonitorInterface.println("Time Update Data RX");
}

void DSCharUpdate(byte * newData, byte length) {
  newDSdata(newData, length);
}

void NSCharUpdate(byte * newData, byte length) {
  newNSdata(newData, length);
}

void BLEConnect() {
  //SerialMonitorInterface.println("---------Connect");
  requestSecurity();
}

void BLEBond() {
  //SerialMonitorInterface.println("---------Bond");
  ANCSInitStep = 0;
}

void BLEDisconnect() {
  //SerialMonitorInterface.println("---------Disconnect");
  ANCSReset();
  ble_connection_state = false;
  ANCSInitStep = -1;
  advertise("TinyWatch", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");
}

void ANCSInit() {
  if (ANCSInitStep == 0)if (!discoverService(&timeService, "1805"))ANCSInitStep++;
  if (ANCSInitStep == 1)if (!discoverService(&ANCSService, "7905F431-B5CE-4E99-A40F-4B1E122D00D0"))ANCSInitStep++;
  if (ANCSInitStep == 2)if (!discoverCharacteristic(&timeService, &currentTimeChar, "2A2B"))ANCSInitStep++;
  if (ANCSInitStep == 3)if (!discoverCharacteristic(&ANCSService, &NSchar, "9FBF120D-6301-42D9-8C58-25E699A21DBD"))ANCSInitStep++;
  if (ANCSInitStep == 4)if (!discoverCharacteristic(&ANCSService, &CPchar, "69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9"))ANCSInitStep++;
  if (ANCSInitStep == 5)if (!discoverCharacteristic(&ANCSService, &DSchar, "22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB"))ANCSInitStep++;
  if (ANCSInitStep == 6)if (!enableNotifications(&currentTimeChar, timeCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 7)if (!readCharacteristic(&currentTimeChar, TimeData, sizeof(TimeData)))ANCSInitStep++;
  if (ANCSInitStep == 8)if (!enableNotifications(&DSchar, DSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 9)if (!enableNotifications(&NSchar, NSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 10) {
    //SerialMonitorInterface.println("Connected!");
    newtime = millisOffset();
    ble_connection_state = true;
  } else {
    ANCSInitRetry = millisOffset();
  }
}*/
