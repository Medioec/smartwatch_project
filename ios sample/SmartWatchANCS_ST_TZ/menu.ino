

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
  currentMenuLine = 0;
  lastMenuLine = -1;
  currentSelectionLine = 0;
  lastSelectionLine = -1;
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

//List of items in menu
static const char PROGMEM mainMenuStrings0[] = "Set date/time";
static const char PROGMEM mainMenuStrings1[] = "Set auto off";
static const char PROGMEM mainMenuStrings2[] = "Set brightness";
static const char PROGMEM mainMenuStrings3[] = "Set Timer";

static const char* const PROGMEM mainMenuStrings[] =
{
  mainMenuStrings0,
  mainMenuStrings1,
  mainMenuStrings2,
  mainMenuStrings3,
};
//Struct for menu, 3rd item is function for handling selection
const menu_info mainMenuInfo =
{
  4,
  mainMenuStrings,
  mainMenu,
};


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
void (*editIntCallBack)() = NULL;

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

void mainMenu(uint8_t selection) {
  if (menu_debug_print)SerialMonitorInterface.println("mainMenuHandler");
  if (selection == 0) {
    newMenu(dateTimeMenuIndex);
  }
  if (selection == 1) {
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &sleepTimeout, buffer, NULL);
  }
  if (selection == 2) {
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &brightness, buffer, NULL);
  }
  if (selection == 3) { //To add functionality for timer
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    user_timer_menu(0, &userTimerSetting, buffer, NULL, &userTimerSetState, &userTimerRunningState);
  }
}


uint8_t dateTimeSelection = 0;
int dateTimeVariable = 0;

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

int timerDigits[6];
int maxTimerDigit = 6;
//uint8_t timerStartTime = millisOffset();

uint8_t user_timer_menu(uint8_t button, int *inVal, char *intName, void (*cb)(), int *setState, int *runState) {
  if (menu_debug_print)SerialMonitorInterface.println("user_timer_menu");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("edit_timer_setting");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
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
      } else {
        display.setCursor(70, menuTextY[0]);
        display.print(F("Pause"));
        display.setCursor(10, menuTextY[1]);
        display.print("Timer Running");
        display.setCursor(75, menuTextY[3]);
        display.print(F("Stop"));
      }
    }
  } else if (button == upButton) {
    if (*setState == 0) {
    if ((currentDigit == 2 or currentDigit == 4)) {
    if (timerDigits[currentDigit] < 5)
      timerDigits[currentDigit]++;
    } else if (timerDigits[currentDigit] < 9)
          timerDigits[currentDigit]++;
    } else {
      if (*runState == 0) {
      *runState = 1;
      display.setCursor(70, menuTextY[0]);
      display.print(F("Pause"));
      display.setCursor(10, menuTextY[1]);
      display.print("Timer Running");
      } else {
      *runState = 0;
      display.setCursor(70, menuTextY[0]);
      display.print(F("Start"));
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
      display.print(F("          "));
      display.setCursor(10, menuTextY[1]);
      display.print(F("Set Timer"));
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
        *setState = 1;
        PRINTF("setState 1\n");
        display.clearWindow(60, 12, 96, 64);
        display.setCursor(0, menuTextY[0]);
        display.print(F("            "));
        display.setFont(font10pt);
        display.fontColor(defaultFontColor, defaultFontBG);
        display.setCursor(70, menuTextY[0]);
        display.print(F("Start"));
        display.setCursor(0, menuTextY[0]);
        display.print(F("< Home"));
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
    }
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
  if (i != currentDigit)display.fontColor(defaultFontColor, defaultFontBG);
  if (i == 1 or i == 3)display.print(F(":"));
  }
  display.print(F("   "));
  return 0;
}