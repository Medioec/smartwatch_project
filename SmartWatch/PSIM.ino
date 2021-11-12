#define TICKMS 50

//uint8_t (*psimHandler)(uint8_t) = NULL;
uint8_t buttonBuffer;
uint8_t displayLineY[7] = {0, 9, 18, 27, 36, 45, 54};
uint8_t buttonY[2] = {12, 51};

int psimMenuLine = 0;

uint8_t psimProcess(uint8_t button)
{
  if (psVarInitLaunch)
  {
    if (!button)
    {
      int exitStatus = 0;
      psimRunState = 1;
      psimMenuLine = 0;
      psimHandler = psimProcess;
      currentDisplayState = displayPSIM;
    
      PRINTF("display state PSIM\n")
      display.clearWindow(0, 0, 96, 64);
      display.setFont(font8pt);
      display.fontColor(defaultFontColor, inactiveFontBG);
      display.setCursor(0, buttonY[0]);
      display.print('<');
      display.setCursor(90, buttonY[0]);
      display.print('^');
      display.setCursor(0, buttonY[1]);
      display.print('>');
      display.setCursor(90, buttonY[1]);
      display.print('v');
      display.setCursor(20, displayLineY[2]);
      display.print("PSIM Test");
    }
    else if (button == TSButtonUpperRight) {
      PRINTF("Pressed\n");
      if (psimMenuLine > 0) {
        psimMenuLine--;
      }
    }
    else if (button == TSButtonLowerRight) {
      if (psimMenuLine < 2) {
        psimMenuLine++;
      }
    }
    else if (button == TSButtonLowerLeft) {
      if (psimMenuLine == 1 && psVarContinueAllowed){
        initpsVarIaunch = 0;
        display.clearWindow(0, 0, 96, 64);
        return 1;
      }
      else if (psimMenuLine == 0) {
        initpsVarIaunch = 0;
        display.clearWindow(0, 0, 96, 64);
        psim_initialise_vars();
        return 1;
      }
      else if (psimMenuLine == 2) {
        psimRunState = 0;
        display.clearWindow(0, 0, 96, 64);
        currentDisplayState = displayStateHome;
        initHomeScreen();
        return 0;
      }
    }
    display.fontColor(inactiveFontColor, inactiveFontBG);
    display.setCursor(20, displayLineY[3]);
    if (psimMenuLine == 0) {
      display.fontColor(defaultFontColor, inactiveFontBG);
    }
      display.print("Start");
    if (psimMenuLine == 0) {
      display.fontColor(inactiveFontColor, inactiveFontBG);
    }

    if (psimMenuLine == 1) {
      display.fontColor(defaultFontColor, inactiveFontBG);
    }
    display.setCursor(20, displayLineY[4]);
    display.print("Continue");
    if (psimMenuLine == 1) {
      display.fontColor(inactiveFontColor, inactiveFontBG);
    }

    if (psimMenuLine == 2) {
      display.fontColor(defaultFontColor, inactiveFontBG);
    }
    display.setCursor(20, displayLineY[5]);
    display.print("Exit");
    if (psimMenuLine == 2) {
      display.fontColor(inactiveFontColor, inactiveFontBG);
    }
  }
  elsepsVarIif (initLaunch == 0) //After Menu
  {
    display.setFont(font8pt);
    display.fontColor(defaultFontColor, inactiveFontBG);
    display.setCursor(0, buttonY[0]);
    display.print('<');
    display.setCursor(70, buttonY[0]);
    display.print("Stats");
    display.setCursor(0, buttonY[1]);
    display.print("Settings");
    display.setCursor(70, buttonY[1]);
    display.print("Alter");
    display.setCursor(0, displayLineY[0]);
    display.print("PSIM Test");
  }
}

void psim_initialise_vars() {
  PRINTF("run load command\n")
  ;
}
