typedef struct
{
  const uint8_t amtLines;
  const char* const * strings;
  void (*selectionHandler)(uint8_t);
} menu_info;


uint8_t menuHistory[5];
uint8_t menuHistoryIndex = 0;
uint8_t currentMenu = 0;
uint8_t currentMenuLine = 0;
uint8_t lastMenuLine = -1;
uint8_t currentSelectionLine = 0;
uint8_t lastSelectionLine = -1;


void newMenu(int8_t newIndex) {
  lastMenuLine = -1;
  lastSelectionLine = -1;
  if (newIndex == 0) {
    currentMenuLine = 0;
    currentSelectionLine = 0;
  }
  if (newIndex >= 0) {
    menuHistory[menuHistoryIndex++] = currentMenu;
    currentMenu = newIndex;
  } else {
    if (currentDisplayState == displayStateMenu) {
      menuHistoryIndex--;
      currentMenu = menuHistory[menuHistoryIndex];
    }
  }
  if (menuHistoryIndex) {
    currentDisplayState = displayStateMenu;
    if (menu_debug_print)SerialMonitorInterface.print("New menu index ");
    if (menu_debug_print)SerialMonitorInterface.println(currentMenu);
  } else {
    if (menu_debug_print)SerialMonitorInterface.print("New menu index ");
    if (menu_debug_print)SerialMonitorInterface.println("home");
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }
}

// -------------------------------------------------------------------
// List of items in menu, add more entries here for more menu entries
// -------------------------------------------------------------------
static const char PROGMEM mainMenuStrings5[] = "Set date/time";
static const char PROGMEM mainMenuStrings6[] = "Set auto off";
static const char PROGMEM mainMenuStrings7[] = "Set brightness";
static const char PROGMEM mainMenuStrings0[] = "Set Timer";
static const char PROGMEM mainMenuStrings1[] = "View Temperature";
static const char PROGMEM mainMenuStrings2[] = "To Do List";
static const char PROGMEM mainMenuStrings3[] = "Dream Startup";
static const char PROGMEM mainMenuStrings4[] = "Tarot";

// Also Update Struct
static const char* const PROGMEM mainMenuStrings[] =
{
  mainMenuStrings0,
  mainMenuStrings1,
  mainMenuStrings2,
  mainMenuStrings3,
  mainMenuStrings4,
  mainMenuStrings5,
  mainMenuStrings6,
  mainMenuStrings7,

};

//Struct for menu, 3rd item is function for handling selection
const menu_info mainMenuInfo =
{
  8, //no. of menu items
  mainMenuStrings, //strings
  mainMenu, //selection handler function
};

// -------------------------------------------------------------------


static const char PROGMEM dateTimeMenuStrings0[] = "Set Year";
static const char PROGMEM dateTimeMenuStrings1[] = "Set Month";
static const char PROGMEM dateTimeMenuStrings2[] = "Set Day";
static const char PROGMEM dateTimeMenuStrings3[] = "Set Hour";
static const char PROGMEM dateTimeMenuStrings4[] = "Set Minute";
static const char PROGMEM dateTimeMenuStrings5[] = "Set Second";

static const char* const PROGMEM dateTimeMenuStrings[] =
{
  dateTimeMenuStrings0,
  dateTimeMenuStrings1,
  dateTimeMenuStrings2,
  dateTimeMenuStrings3,
  dateTimeMenuStrings4,
  dateTimeMenuStrings5,
};

const menu_info dateTimeMenuInfo =
{
  6,
  dateTimeMenuStrings,
  dateTimeMenu,
};

const menu_info menuList[] = {mainMenuInfo, dateTimeMenuInfo};
#define mainMenuIndex 0
#define dateTimeMenuIndex 1

int currentVal = 0;
int digits[4];
int currentDigit = 0;
int maxDigit = 4;
int *originalVal;


uint8_t dateTimeSelection = 0;
int dateTimeVariable = 0;

// -------------------------------------------------------------------
// Edit Date Time Menu
// -------------------------------------------------------------------
uint8_t editInt(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = editInt;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    digits[3] = currentVal % 10; currentVal /= 10;
    digits[2] = currentVal % 10; currentVal /= 10;
    digits[1] = currentVal % 10; currentVal /= 10;
    digits[0] = currentVal % 10;
    currentVal = *originalVal;
    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);
    display.setCursor(0, menuTextY[0]);
    display.print(F("< back/undo"));
    display.setCursor(90, menuTextY[0]);
    display.print('^');
    display.setCursor(10, menuTextY[1]);
    display.print(intName);
    display.setCursor(0, menuTextY[3]);
    display.print(F("> next/save"));
    display.setCursor(90, menuTextY[3]);
    display.print('v');
  } else if (button == upButton) {
    if (digits[currentDigit] < 9)
      digits[currentDigit]++;
  } else if (button == downButton) {
    if (digits[currentDigit] > 0)
      digits[currentDigit]--;
  } else if (button == selectButton) {
    if (currentDigit < maxDigit - 1) {
      currentDigit++;
    } else {
      //save
      int newValue = (digits[3]) + (digits[2] * 10) + (digits[1] * 100) + (digits[0] * 1000);
      *originalVal = newValue;
      viewMenu(backButton);
      if (editIntCallBack) {
        editIntCallBack();
        editIntCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentDigit > 0) {
      currentDigit--;
    } else {
      if (menu_debug_print)SerialMonitorInterface.println(F("back"));
      viewMenu(backButton);
      return 0;
    }
  }
  display.setCursor(10, menuTextY[2]);
  for (uint8_t i = 0; i < 4; i++) {
    if (i != currentDigit)display.fontColor(inactiveFontColor, defaultFontBG);
    display.print(digits[i]);
    if (i != currentDigit)display.fontColor(defaultFontColor, defaultFontBG);
  }
  display.print(F("   "));
  return 0;
}

// -------------------------------------------------------------------
// Main Menu Selection Handler
// -------------------------------------------------------------------
void mainMenu(uint8_t selection) {
  if (menu_debug_print)SerialMonitorInterface.println("mainMenuHandler");
  if (selection == 5) {
    newMenu(dateTimeMenuIndex);
  }
  if (selection == 6) {
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &sleepTimeout, buffer, NULL);
  }
  if (selection == 7) {
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &brightness, buffer, NULL);
  }
  if (selection == 0) { //code for timer
    user_timer_menu(0, &userTimerSetting, 0, NULL, &userTimerSetState, &userTimerRunningState);
  }
  if (selection == 3) { //To add functionality for timer
    psimProcess(0);
  }
  if (selection == 1) {
    char buffer[20];
    viewtemp(0, NULL, 0, NULL);
  }
  if (selection == 2) {
    ToDoListStart();
  }
  if (selection == 4) {
    Tarot(0, NULL, 0, NULL);//tarot
  }
}



void saveChangeCallback() {
#if defined (ARDUINO_ARCH_AVR)
  int timeData[] = {year(), month(), day(), hour(), minute(), second()};
  timeData[dateTimeSelection] = dateTimeVariable;
  setTime(timeData[3], timeData[4], timeData[5], timeData[2], timeData[1], timeData[0]);
#elif defined(ARDUINO_ARCH_SAMD)
  int timeData[] = {RTCZ.getYear(), RTCZ.getMonth(), RTCZ.getDay(), RTCZ.getHours(), RTCZ.getMinutes(), RTCZ.getSeconds()};
  timeData[dateTimeSelection] = dateTimeVariable;
  RTCZ.setTime(timeData[3], timeData[4], timeData[5]);
  RTCZ.setDate(timeData[2], timeData[1], timeData[0] - 2000);
#endif
  if (menu_debug_print)SerialMonitorInterface.print("set time ");
  if (menu_debug_print)SerialMonitorInterface.println(dateTimeVariable);
}


void dateTimeMenu(uint8_t selection) {
  if (menu_debug_print)SerialMonitorInterface.print("dateTimeMenu ");
  if (menu_debug_print)SerialMonitorInterface.println(selection);
  if (selection >= 0 && selection < 6) {
#if defined (ARDUINO_ARCH_AVR)
    int timeData[] = {year(), month(), day(), hour(), minute(), second()};
#elif defined(ARDUINO_ARCH_SAMD)
    int timeData[] = {RTCZ.getYear(), RTCZ.getMonth(), RTCZ.getDay(), RTCZ.getHours(), RTCZ.getMinutes(), RTCZ.getSeconds()};
#endif
    dateTimeVariable = timeData[selection];
    dateTimeSelection = selection;
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[dateTimeMenuIndex].strings[selection])));
    editInt(0, &dateTimeVariable, buffer, saveChangeCallback);
  }
}

// -------------------------------------------------------------------
// View Menu
// -------------------------------------------------------------------
void viewMenu(uint8_t button) {
  if (menu_debug_print)SerialMonitorInterface.print("viewMenu ");
  if (menu_debug_print)SerialMonitorInterface.println(button);
  if (!button) {
    newMenu(mainMenuIndex);
    display.clearWindow(0, 12, 96, 64);
  } else {
    if (button == upButton) {
      if (currentSelectionLine > 0) {
        currentSelectionLine--;
      } else if (currentMenuLine > 0) {
        currentMenuLine--;
      }
    } else if (button == downButton) {
      if (currentSelectionLine < menuList[currentMenu].amtLines - 1 && currentSelectionLine < 3) {
        currentSelectionLine++;
      } else if (currentSelectionLine + currentMenuLine < menuList[currentMenu].amtLines - 1) {
        currentMenuLine++;
      }
    } else if (button == selectButton) {
      if (menu_debug_print)SerialMonitorInterface.print("select ");
      if (menu_debug_print)SerialMonitorInterface.println(currentMenuLine + currentSelectionLine);
      menuList[currentMenu].selectionHandler(currentMenuLine + currentSelectionLine);
    } else if (button == backButton) {
      newMenu(-1);
      if (!menuHistoryIndex)
        return;
    }
  }
  display.setFont(font10pt);
  if (lastMenuLine != currentMenuLine || lastSelectionLine != currentSelectionLine) {
    if (menu_debug_print)SerialMonitorInterface.println("drawing menu ");
    if (menu_debug_print)SerialMonitorInterface.println(currentMenu);
    for (int i = 0; i < 4; i++) {
      display.setCursor(7, menuTextY[i]);
      if (i == currentSelectionLine) {
        display.fontColor(defaultFontColor, inactiveFontBG);
      } else {
        display.fontColor(inactiveFontColor, inactiveFontBG);
      }
      if (currentMenuLine + i < menuList[currentMenu].amtLines) {
        char buffer[20];
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[currentMenu].strings[currentMenuLine + i])));
        display.print(buffer);
      }
      for (uint8_t i = 0; i < 25; i++)display.write(' ');
      if (i == 0) {
        display.fontColor(defaultFontColor, inactiveFontBG);
        display.setCursor(0, menuTextY[0]);
        display.print('<');
        display.setCursor(90, menuTextY[0]);
        display.print('^');
      }
      if (i == 3) {
        display.fontColor(defaultFontColor, inactiveFontBG);
        display.setCursor(0, menuTextY[3]);
        display.print('>');
        display.setCursor(90, menuTextY[3]);
        display.print('v');
      }
    }
    lastMenuLine = currentMenuLine;
    lastSelectionLine = currentSelectionLine;
  }
}


// -------------------------------------------------------------------
// User Timer Menu
// -------------------------------------------------------------------
int timerDigits[6];

uint8_t user_timer_menu(uint8_t button, int *inVal, char *intName, void (*cb)(), int *setState, int *runState) {
  //clear stored display digits
  reset_timer_display();
  int maxTimerDigit = 6;
  if (menu_debug_print)SerialMonitorInterface.println("user_timer_menu");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("edit_timer_setting");
    editIntCallBack = cb;
    currentDisplayState = displayStateTimer;
    userTimerHandler = user_timer_menu;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    timerDigits[5] = currentVal % 10; currentVal /= 10;
    timerDigits[4] = currentVal % 6; currentVal /= 6;
    timerDigits[3] = currentVal % 10; currentVal /= 10;
    timerDigits[2] = currentVal % 6; currentVal /= 6;
    timerDigits[1] = currentVal % 10; currentVal /= 10;
    timerDigits[0] = currentVal % 10;
    currentVal = *originalVal;
    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);
    if (*setState == 0) {
      display.setCursor(0, menuTextY[0]);
      display.print(F("< back/undo"));
      display.setCursor(90, menuTextY[0]);
      display.print('^');
      display.setCursor(10, menuTextY[1]);
      display.print("Set Timer");
      display.setCursor(0, menuTextY[3]);
      display.print(F("> next/save"));
      display.setCursor(90, menuTextY[3]);
      display.print('v');
    } else {
      display.setCursor(0, menuTextY[0]);
      display.print(F("< Home"));
      if (*runState == 0) {
        display.setCursor(70, menuTextY[0]);
        display.print(F("Start"));
        display.setCursor(10, menuTextY[1]);
        display.print("Timer Set");
        display.setCursor(75, menuTextY[3]);
        display.print(F("Stop"));
        update_user_timer(userTimerCurrentValue);
        return 0;
      } else {
        display.setCursor(70, menuTextY[0]);
        display.print(F("Pause"));
        display.setCursor(10, menuTextY[1]);
        display.print("Timer Running");
        display.setCursor(75, menuTextY[3]);
        display.print(F("Stop"));
        update_user_timer(userTimerLastValue);
        return 0;
      }
    }
  } else if (button == upButton) {
    if (*setState == 0) {
      if ((currentDigit == 2 or currentDigit == 4)) {
        if (timerDigits[currentDigit] < 5)
          timerDigits[currentDigit]++;
      }
      else if (timerDigits[currentDigit] < 9)
        timerDigits[currentDigit]++;
    } else {
      if (*runState == 0) {
        *runState = 1;
        display.setCursor(70, menuTextY[0]);
        display.print(F("Pause"));
        display.setCursor(10, menuTextY[1]);
        display.print("Timer Running");
        userTimerStartTime = millisOffset();
        return 0;
      } else {
        if (!userTimerCurrentValue) return 0;
        *runState = 0;
        userTimerLastValue = userTimerCurrentValue;
        display.setCursor(70, menuTextY[0]);
        display.print(F("Start"));
        display.setCursor(10, menuTextY[1]);
        display.print("Timer Paused   ");
        return 0;
      }
    }

  } else if (button == downButton) {
    if (*setState == 0) {
      if (timerDigits[currentDigit] > 0)
        timerDigits[currentDigit]--;
    } else {
      *runState = 0;
      *setState = 0;
      display.clearWindow(60, 12, 96, 64);
      display.setCursor(0, menuTextY[0]);
      display.print(F("< back/undo"));
      display.setCursor(90, menuTextY[0]);
      display.print('^');
      display.setCursor(10, menuTextY[1]);
      display.print(F("Set Timer    "));
      display.setCursor(0, menuTextY[3]);
      display.print(F("> next/save"));
      display.setCursor(90, menuTextY[3]);
      display.print('v');
    }
  } else if (button == selectButton) {
    if (*setState == 0) {
      PRINTF("next digit\n");
      if (currentDigit < maxTimerDigit - 1) currentDigit++;
      else {
        //save
        int newValue = (timerDigits[5]) + (timerDigits[4] * 10) + (timerDigits[3] * 60) + \
                       (timerDigits[2] * 600) + (timerDigits[1] * 3600) + (timerDigits[0] * 36000);
        *originalVal = newValue;
        userTimerLastValue = newValue * 1000;
        userTimerCurrentValue = newValue * 1000;
        *setState = 1;
        display.clearWindow(60, 12, 96, 64);
        display.setCursor(0, menuTextY[0]);
        display.setFont(font10pt);
        display.fontColor(defaultFontColor, defaultFontBG);
        display.setCursor(70, menuTextY[0]);
        display.print(F("Start"));
        display.setCursor(0, menuTextY[0]);
        display.print(F("< Home       "));
        display.setCursor(10, menuTextY[1]);
        display.print(F("Timer Set"));
        display.setCursor(75, menuTextY[3]);
        display.print(F("Stop"));
        display.setCursor(0, menuTextY[3]);
        display.print(F("           "));

        /*if (editIntCallBack) {
          editIntCallBack();
          editIntCallBack = NULL;
          }*/
        //return 0;
      }
    } else return 0;
  } else if (button == backButton) {
    if (*setState == 0) {
      SerialMonitorInterface.println(*setState);
      if (currentDigit > 0) {
        currentDigit--;
      } else {
        SerialMonitorInterface.println(*setState);
        if (menu_debug_print)SerialMonitorInterface.println(F("back"));
        viewMenu(backButton);
        return 0;
      }
    } else {
      menuHistoryIndex = 0;
      currentDisplayState = displayStateHome;
      initHomeScreen();
      return 1;
    }

  }
  display.setCursor(10, menuTextY[2]);
  for (uint8_t i = 0; i < 6; i++) {
    if (*setState == 0) {
      if (i != currentDigit)display.fontColor(inactiveFontColor, defaultFontBG);
    } else {
      if (i != currentDigit)display.fontColor(defaultFontColor, defaultFontBG);
    }
    display.print(timerDigits[i]);
    lastTimerDisplayed[i] = timerDigits[i];
    if (i != currentDigit)display.fontColor(defaultFontColor, defaultFontBG);
    if (i == 1 || i == 3)display.print(F(":"));
  }
  display.print(F("   "));
  return 0;
}

// -------------------------------------------------------------------
// View Temperature
// -------------------------------------------------------------------
uint8_t viewtemp(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = editInt;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;

    digits[3] = currentVal % 10; currentVal /= 10;
    digits[2] = currentVal % 10; currentVal /= 10;
    digits[1] = currentVal % 10; currentVal /= 10;
    digits[0] = currentVal % 10;
    currentVal = *originalVal;
    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);

    display.setCursor(0, menuTextY[0]);
    display.print(F("< back"));
    display.setCursor(90, menuTextY[0]);
    display.setCursor(10, menuTextY[1]);
    showSerial();
    display.setCursor(0, menuTextY[3]);



  } else if (button == upButton) {
    if (digits[currentDigit] < 9)
      digits[currentDigit]++;
  } else if (button == downButton) {
    if (digits[currentDigit] > 0)
      digits[currentDigit]--;
  } else if (button == selectButton) {
    if (currentDigit < maxDigit - 1) {
      currentDigit++;
    } else {
      //save

      viewMenu(backButton);
      if (editIntCallBack) {
        editIntCallBack();
        editIntCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentDigit > 0) {
      currentDigit--;
    } else {
      if (menu_debug_print)SerialMonitorInterface.println(F("back"));
      viewMenu(backButton);
      return 0;
    }
  }



  return 0;
}


//------------------------------------------------------
// ToDoList
//------------------------------------------------------

#define MAX_CHARS 20 // Max Characters
#define MAX_ITEMS 4 // Max  To Do List Items

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

#include <STBLE.h>
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

bool dataSent = false;

void addItem(char bufferArray[][MAX_CHARS + 1], char* item, int* currCount, int* ErrorState);
void printDisplay(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currSize, int* currX, int* currY);
void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState, int* selectedIndex, int* currCount, int* currX, int* currY, char bufferArray[MAX_ITEMS][MAX_CHARS + 1], int* exitList);
void error1(int* currX, int* currY, int* ErrorState);

// 94x64 RGB Pixels

// Button UpperLeft -> MENU
// Button LowerLeft -> DELETE
// Button LowerRight -> Up
// Button UpperRight -> Down

char bufferArray[MAX_ITEMS][MAX_CHARS + 1]; // Array to store all items for ToDoList
//------------------------------------------------------
// ToDoList - Main Loop
//------------------------------------------------------
void ToDoListStart() {
  int currX = 0;                      // Current X pointer
  int currY = 12;                      // Current Y pointer
  int exitList = 0;

  char* bufferString = (char*)malloc(MAX_CHARS * sizeof(char));


  int selectedIndex = 0;                      // Index that is currently selected.
  int currCount = 0;                          // Current count of array

  // Button states, 1 => pressed, 0 => not pressed
  int LowerLeftState = 0;
  int UpperLeftState = 0;
  int LowerRightState = 0;
  int UpperRightState = 0;

  // Error state, 0 => no error, 1 => too many items
  int ErrorState = 0;

  currentDisplayState = displayStateToDo;


  addItem(bufferArray, "Walk the Dog", &currCount, &ErrorState);
  addItem(bufferArray, "Walk the Dog", &currCount, &ErrorState);
  addItem(bufferArray, "Walk the Dog", &currCount, &ErrorState);
  printDisplay(bufferArray, &selectedIndex, &currCount, &currX, &currY);

  //display.setBrightness(10);
  display.setFlip(true);
  display.setCursor(currX, currY);

  while (1) {
    if (exitList) {
      display.clearWindow(0, 12, 96, 64);
      display.setFont(font10pt);
      break;
    }

    checkButtonStates(&LowerLeftState, &UpperLeftState, &LowerRightState, &UpperRightState, &selectedIndex, &currCount, &currX, &currY, bufferArray, &exitList);  // Check for button inputs.
    checkBluetooth(bufferString); // Check for bluetooth inputs, if avail, send to buffer string

    // Handle bluetooth input
    if (dataSent) {
      // If bluetooth input received, create new task on watch.
      addItem(bufferArray, bufferString, &currCount, &ErrorState);
      dataSent = false;

      // Handle too many items error
      if (ErrorState == 1) {
        error1(&currX, &currY, &ErrorState);
        ErrorState = 0;
      }

      // Reprint the screen, with new item

      // Clear screen, reset X and Y pointers
      display.clearWindow(0, 12, 96, 64);
      currX = 0;
      currY = 12;

      // Print current bufferarray
      printDisplay(bufferArray, &selectedIndex, &currCount, &currX, &currY);
    }
  }
  display.clearWindow(0, 12, 96, 64);
  viewMenu(backButton);
  currentDisplayState = displayStateMenu;
  drawDateBar();
  return;
}

//------------------------------------------------------
// ToDoList - Print Too Many Items Error, (error 1)
//------------------------------------------------------
void error1(int* currX, int* currY, int* ErrorState) {
  int tmpX = 0;
  int tmpY = 12;

  char* errorText = "ERROR!!!";
  char* errorText2 = "More than 4 items detected";

  display.clearWindow(0, 12, 96, 64);
  display.setCursor(tmpX, tmpY);
  display.setFont(liberationSans_10ptFontInfo);
  display.fontColor(RED, BLACK);

  display.print(errorText);
  int fontHeight = display.getFontHeight();
  tmpY += fontHeight;
  display.setCursor(tmpX, tmpY);
  display.print(errorText2);

  delay(2000); // Error display length
  display.setCursor(*currX, *currY); // Move cursor back to original location
}

//------------------------------------------------------
// ToDoList - Print display on watch
//------------------------------------------------------
void printDisplay(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currSize, int* currX, int* currY) {
  for (int i = 0; i < *currSize; i++) {
    display.setCursor(*currX, *currY);
    char* currString = *(bufferArray + i); // Current string iteration
    if (i == *selectedIndex) {
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
// ToDoList - Adds Item to Array
//------------------------------------------------------
void addItem(char bufferArray[][MAX_CHARS + 1], char* item, int* currCount, int* ErrorState) {
  if (*currCount < MAX_ITEMS) {
    char newString[MAX_CHARS + 1] = "> "; // Adds "-" before to do item
    strcat(newString, item);
    strcpy(*(bufferArray + *currCount), newString); // Add string to array
    *currCount += 1;                                // Add to current index
  } else {
    *ErrorState = 1; // Set too many items error
  }
}

//------------------------------------------------------
// ToDoList - Removes Item from Array
//------------------------------------------------------
void removeItem(char bufferArray[][MAX_CHARS + 1], int* selectedIndex, int* currCount) {
  // Remove select item index from array.
  if (*currCount > 0) {
    bool deleted = false;
    for (int i = 0; i < MAX_ITEMS; i++) {

      if (i == *selectedIndex) {
        deleted = true;
      }
      if (deleted) {
        // If index in range, shift next index in
        if (i + 1 <= MAX_ITEMS - 1) {
          strcpy(bufferArray[i], bufferArray[i + 1]);
        }
      }
    }

    *selectedIndex = 0;
    *currCount -= 1;
  }
}

//------------------------------------------------------
// ToDoList - Check for Bluetooth Input, if avail, save to bufferstring
//------------------------------------------------------
void checkBluetooth (char* myBufferString) {
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
// ToDoList - Check for Button Presses, if found execute
//------------------------------------------------------
void checkButtonStates(int* LowerLeftState, int* UpperLeftState, int* LowerRightState, int* UpperRightState, int* selectedIndex, int* currCount, int* currX, int* currY, char bufferArray[MAX_ITEMS][MAX_CHARS + 1], int* exitList) {
  // If Upper Left Button is pressed.
  if (display.getButtons(TSButtonUpperLeft)) {
    *UpperLeftState = 1;
    if (UpperLeftState != 0) {
      delay(500); // Buffer for button press.

      *UpperLeftState = 0;
      *exitList = 1;
      return;
    }
  }

  // If Upper Right Button is pressed.
  if (display.getButtons(TSButtonUpperRight)) {
    *UpperRightState = 1;
    if (*UpperRightState != 0) {
      delay(500); // Buffer for button press.

      // Select Previous
      *selectedIndex -= 1;
      if (*selectedIndex < 0) {
        // If out of bounds, go back to first index.
        *selectedIndex = 0;
      }

      // Clear screen, reset X and Y pointers
      display.clearWindow(0, 12, 96, 64);
      *currX = 0;
      *currY = 12;

      // Reprint the screen, with new item
      printDisplay(bufferArray, selectedIndex, currCount, currX, currY);
      *UpperRightState = 0;
    }
  }

  // If Lower Left Button is pressed.
  if (display.getButtons(TSButtonLowerLeft)) {
    *LowerLeftState = 1;
    if (*LowerLeftState != 0) {
      delay(500); // Buffer for button press.
      // Remove items
      removeItem(bufferArray, selectedIndex, currCount);

      // Clear screen, reset X and Y pointers
      display.clearWindow(0, 12, 96, 64);
      *currX = 0;
      *currY = 12;

      // Reprint the screen, with new item

      printDisplay(bufferArray, selectedIndex, currCount, currX, currY);


      *LowerLeftState = 0;
    }
  }

  // If Lower Right Button is pressed.
  if (display.getButtons(TSButtonLowerRight)) {
    *LowerRightState = 1;
    if (*LowerRightState != 0) {
      delay(500); // Buffer for button press.

      // NEXT
      // Select Next To Do List
      *selectedIndex += 1;
      if (*selectedIndex > *currCount - 1) {
        // If out of bounds, go back to first index.
        *selectedIndex = 0;
      }

      // Clear screen, reset X and Y pointers
      display.clearWindow(0, 12, 96, 64);
      *currX = 0;
      *currY = 12;

      // Reprint the screen, with new item
      printDisplay(bufferArray, selectedIndex, currCount, currX, currY);

      *LowerRightState = 0;
    }
  }
}

//------------------------------------------------------
// Tarot Card App
//------------------------------------------------------
uint8_t Tarot(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  //if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    //if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = 0x99;
    editorHandler = Tarot;

    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);

     display.setCursor(0, menuTextY[0]);
    display.print(F("< back"));
    display.setCursor(90, menuTextY[0]);
    display.setCursor(10, menuTextY[1]);
    showCard();
    display.setCursor(0, menuTextY[3]);
  } 
  if (button == downButton) {
    showCard();
    return 0;
  } 
  else if (button == selectButton) {
      showCard();
      return 1;
    
  } else if (button == backButton) {

      //if (menu_debug_print)SerialMonitorInterface.println(F("back"));
      
      viewMenu(backButton);
      return 0;
    
  }
  return 0;
}
