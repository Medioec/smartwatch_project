#define TICKMS 50

//uint8_t (*psimHandler)(uint8_t) = NULL;
uint8_t buttonBuffer;
uint8_t displayLineY[7] = {0, 9, 18, 27, 36, 45, 54};
uint8_t buttonY[2] = {12, 51};

int psimMenuLine = 0;
char percentString[6] = "";
char scoreString[4] = "";

uint8_t psimProcess(uint8_t button)
{
  display.setFont(font8pt);
  psimHandler = psimProcess;
  currentDisplayState = displayPSIM;
  if (psVarInitLaunch)
  {
    if (!button)
    {
      int exitStatus = 0;
      psimRunState = 1;
      psimMenuLine = 0;
    
      PRINTF("display state PSIM\n")
      display.clearWindow(0, 0, 96, 64);
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
    else if (button == TSButtonUpperLeft) {
      display.clearWindow(0, 0, 96, 64);
      viewMenu(backButton);
      return 1;
    }
    else if (button == TSButtonLowerLeft) {
      if (psimMenuLine == 1 && psVarContinueAllowed){
        psVarInitLaunch = 0;
        display.clearWindow(0, 0, 96, 64);
        drawPsimMenu();
        return 1;
      }
      else if (psimMenuLine == 0) {
        psVarInitLaunch = 0;
        display.clearWindow(0, 0, 96, 64);
        drawPsimMenu();
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
  else if (psVarInitLaunch == 0) //After Menu
  {
    if (!button) {
      display.clearWindow(0, 0, 96, 64);
      drawPsimMenu();
    }
    else if (button == TSButtonUpperLeft) {
      display.clearWindow(0, 0, 96, 64);
      psVarInitLaunch = 1;
      psimProcess(0);
      return 1;
    }
    else if (button == TSButtonUpperRight) {
      ;
    }
    else if (button == TSButtonLowerRight) {
      ;
    }
    else if (button == TSButtonLowerLeft) {
      ;
    }

  }
}

void tickGame() {
  if ( (millisOffset() - psRelTime) > 100) {
    updateVars();
  }
  psRelTime = millisOffset();
}

void psim_initialise_vars() {
  PRINTF("run load command\n")
  //PVAR
  psVarScore = 0;
  psVarSpreadability = 0;
  psVarSeverity = 0;
  psVarhotAug = 0;
  psVarColdAug = 0;

  //AVAR
  psVarNumTotal = 7900000000;
  psVarNumSpread = 1;
  psVarSpreadResist = 0;
  psVarCounterProgress = 0;
  psVarSrUpProb = 0;
  psVarCpUpProb = 0;
}

void updateVars() {
  if (psVarNumSpread < psVarNumTotal) {
    psVarNumSpread = 200000 + psVarNumSpread * (2 + psVarSpreadability/100 - psVarSpreadResist/100 );
    if (psVarNumSpread > psVarNumTotal) {psVarNumSpread = psVarNumTotal;}
  }
  
}

void updatePsimDisplay() {
  if (psVarInitLaunch == 0) {
    float percentSpread;
    percentSpread = (psVarNumSpread/psVarNumTotal);
    char newstring[6];
    snprintf(newstring, 6, "%.1f%%", percentSpread);
    if (1) {
      snprintf(percentString, 6, "%.1f%% ", percentSpread);
      display.setCursor(60, displayLineY[2]);
      display.print(percentString);
    }
    char newscore[4];
    snprintf(newscore, 4, "%i", psVarScore);
    if (1) {
      snprintf(scoreString, 4, "%i", psVarScore);
      display.setCursor(60, displayLineY[3]);
      display.print(scoreString);
    }
    
    }
    
}

void drawPsimMenu() {
  display.fontColor(defaultFontColor, inactiveFontBG);
  display.setCursor(70, buttonY[1]);
  display.print("Stats");
  display.setCursor(0, buttonY[1]);
  display.print("Settings");
  display.setCursor(70, displayLineY[0]);
  display.print("Alter");
  display.setCursor(0, displayLineY[0]);
  display.print("PSIM Test");

  display.setCursor(0, displayLineY[2]);
  display.print("% Infected: ");

  display.setCursor(0, displayLineY[3]);
  display.print("Points: ");
  
  updatePsimDisplay();

}
