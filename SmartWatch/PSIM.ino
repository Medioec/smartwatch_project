#define TICKMS 50

//uint8_t (*psimHandler)(uint8_t) = NULL;
uint8_t buttonBuffer;
uint8_t displayLineY[7] = {0, 9, 18, 27, 36, 45, 54};
uint8_t buttonY[2] = {12, 48};

int psimMenuLine = 0;
char empCountString[4] = "";
char fundString[8] = "";
int windowOpen = 0;
int settingsOpen = 0;
int upgradeOpen = 0;
int statsOpen = 0;
int tickMs = 1000;
int menuSelect = 0;
int menuOffset = 0;
int menuOptionNum = 5;
char upgradeOptionArr[5][20] = {"Intern", "DipGrad", "Grad", "Computer", "Lounge"};
int upgradeCostArr[5] = {intern.cost, dipGrad.cost, grad.cost, computer.cost, lounge.cost};

uint8_t psimProcess(uint8_t button)
{
  display.setFont(font10pt);
  psimHandler = psimProcess;
  currentDisplayState = displayPSIM;
  if (psVarInitLaunch)
  {
    if (!button)
    {
      int exitStatus = 0;
      psimRunState = 1;
    
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
      display.setCursor(20, displayLineY[0]);
      display.fontColor(TS_8b_Yellow, inactiveFontBG);
      display.print("The Dream");
      display.setCursor(25, displayLineY[1]);
      display.print("Startup");
      display.fontColor(defaultFontColor, inactiveFontBG);
    }
    else if (button == TSButtonUpperRight) {
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
        psVarContinueAllowed = 1;
        psimMenuLine = 1;
        return 1;
      }
      else if (psimMenuLine == 2) {
        psimRunState = 0;
        display.clearWindow(0, 0, 96, 64);
        currentDisplayState = displayStateHome;
        psimMenuLine = 0;
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
    display.print("Stop Game");
    if (psimMenuLine == 2) {
      display.fontColor(inactiveFontColor, inactiveFontBG);
    }
  }
  else if (psVarInitLaunch == 0) //After Menu
  {
    if (!button) {
      drawPsimMenu();
    }
    else if (button == TSButtonUpperLeft) {
      if (windowOpen == 0 && settingsOpen == 0 && upgradeOpen == 0 && statsOpen == 0) {
        psVarInitLaunch = 1;
        display.clearWindow(0, 0, 96, 64);
        psVarInitLaunch = 1;
        psimProcess(0);
        return 1;
      } else {
        windowOpen = 0;
        settingsOpen = 0;
        upgradeOpen = 0;
        statsOpen = 0;
        drawPsimMenu();
      }

    }
    else if (button == TSButtonUpperRight) {
      if (!windowOpen && !upgradeOpen) {
        windowOpen = 1;
        upgradeOpen = 1;
        drawWindow();
        drawUpgradeMenu();
        drawUpgradeSelect();
      } else if (upgradeOpen) {
        if (menuSelect > 0) {
          menuSelect--;
        } else if (menuOffset > 0) {
          menuOffset--;
        }
        drawUpgradeSelect();
      }

    }
    else if (button == TSButtonLowerRight) {
      if (!settingsOpen && !windowOpen){
        windowOpen = 1;
        statsOpen = 1;
        drawWindow();
        drawStatsMenu();
      } else if (settingsOpen) {
        if (psVarspeed < 3) {
          psVarspeed++;
          tickMs -= 500;
          display.setCursor(60, displayLineY[2]);
          display.print(psVarspeed);
        }
      } else if (upgradeOpen) {
        if (menuSelect < 2) {
          menuSelect++;
        } else if (menuOffset < 2) {
          menuOffset++;
        }
        drawUpgradeSelect();
      }
    }
    else if (button == TSButtonLowerLeft) {
      if (!settingsOpen && !windowOpen){
        windowOpen = 1;
        settingsOpen = 1;
        drawWindow();
        drawSettingsMenu();
      } else if (settingsOpen) {
        if (psVarspeed > 1) {
          psVarspeed--;
          tickMs += 500;
          display.setCursor(60, displayLineY[2]);
          display.print(psVarspeed);
        }
      } else if (upgradeOpen) {
        int selection = menuOffset + menuSelect;
        if (player.funds - upgradeCostArr[selection] >= 0) {
          player.funds -= upgradeCostArr[selection];
          switch (selection) {
            case 0:
            if (intern.number <= 99) {
              intern.number += 1;
            }
            break;
            case 1:
            if (dipGrad.number <= 99) {
              dipGrad.number += 1;
            }
            break;
            case 2:
            if (grad.number <= 99) {
              grad.number += 1;
            }
            break;
            case 3:
            if (computer.level <= 99) {
              computer.level += 1;
              computer.cost = (player.manning) * 2000;
              upgradeCostArr[selection] = computer.cost;
              computer.modifier = 0.1 * computer.level;
            }
            break;
            case 4:
            if (lounge.level <= 99) {
              lounge.level += 1;
              lounge.cost = (1 + lounge.level) * 2000;
              upgradeCostArr[selection] = lounge.cost;
              lounge.modifier = 0.1 * lounge.level;
            }
            break;
          }
          drawUpgradeSelect();
        }
      }
    }
  }
}



void psim_initialise_vars() {
  player = (startup){.funds = 0, .manning = 1};

  intern = (employee){.income = 10, .cost = 800, .number = 1};
  dipGrad = (employee){.income = 50, .cost = 2000, .number = 0};
  grad = (employee){.income = 200, .cost = 4000, .number = 0};

  computer = (upgrades) {.modifier = 0.1, .cost = 2000, .level = 0};
  lounge = (upgrades) {.modifier = 0.1, .cost = 2000, .level = 0};
}

void updateVars() {
  if (player.funds <= 999999999999) {
    player.funds += (intern.income * intern.number + dipGrad.income * dipGrad.number + grad.income * grad.number) * (1 + lounge.modifier + computer.modifier);
  }
  player.manning = intern.number + dipGrad.number + grad.number;
  if (currentDisplayState == displayPSIM) {updatePsimDisplay();}
}

void tickGame() {
  if ( (millisOffset() - psRelTime) > tickMs) {
    updateVars();
    psRelTime = millisOffset();
  }
}

void updatePsimDisplay() {
  if (psVarInitLaunch == 0 && windowOpen == 0) {
    if (player.funds/1000000000 >1) {
      snprintf(fundString, 8, "%6.2fB", player.funds/1000000000);
    } else if (player.funds/1000000 >1) {
      snprintf(fundString, 8, "%6.2fM", player.funds/1000000);
    } else if (player.funds/1000 >1) {
      snprintf(fundString, 8, "%6.2fK", player.funds/1000);
    } else {
      snprintf(fundString, 8, "%6.2f", player.funds);
    }
    display.setCursor(50, displayLineY[3]);
    display.print(fundString);

    char currEmploy[4];
    snprintf(currEmploy, 4, "%i", player.manning);
    if (currEmploy != empCountString) {
      snprintf(empCountString, 4, "%i", player.manning);
      display.setCursor(60, displayLineY[2]);
      display.print(empCountString);
    }
  } else if (upgradeOpen) {
    if (player.funds/1000000000 >1) {
      snprintf(fundString, 8, "%6.2fB", player.funds/1000000000);
    } else if (player.funds/1000000 >1) {
      snprintf(fundString, 8, "%6.2fM", player.funds/1000000);
    } else if (player.funds/1000 >1) {
      snprintf(fundString, 8, "%6.2fK", player.funds/1000);
    } else {
      snprintf(fundString, 8, "%6.2f", player.funds);
    }
    display.setCursor(45, displayLineY[4]);
    display.print(fundString);
  }
}

void drawPsimMenu() {
  display.clearWindow(0, 0, 96, 64);
  display.fontColor(TS_8b_Blue, inactiveFontBG);
  display.setCursor(70, buttonY[1]);
  display.print("Stats");
  display.setCursor(0, buttonY[1]);
  display.print("Settings");
  display.setCursor(70, displayLineY[0]);
  display.print("Upgrd");
  display.fontColor(defaultFontColor, inactiveFontBG);

  display.setCursor(0, displayLineY[0]);
  display.fontColor(TS_8b_Yellow, inactiveFontBG);
  display.print("Your Startup");
  display.fontColor(TS_8b_White, inactiveFontBG);

  display.setCursor(0, displayLineY[2]);
  display.print("Employees:");

  display.setCursor(0, displayLineY[3]);
  display.print("Funds:");
  
  updatePsimDisplay();

}

void drawWindow() {
  display.clearWindow(0, 0, 96, 64);
  display.fontColor(defaultFontColor, inactiveFontBG);
  display.drawLine(0,0,0,63,TS_16b_White);
  display.drawLine(0,0,95,0,TS_16b_White);
  display.drawLine(95,0,95,63,TS_16b_White);
  display.drawLine(0,63,95,63,TS_16b_White);
}

void drawStatsMenu () {
  char loungebuffer[6] = "";
  char computerbuffer[6] = "";
  snprintf(computerbuffer, 6, "Lv %i", computer.level);
  snprintf(loungebuffer, 6, "Lv %i", lounge.level);
  display.setCursor(5, displayLineY[0]);
  display.fontColor(TS_8b_Yellow, inactiveFontBG);
  display.print("Stats");
  display.fontColor(TS_8b_White, inactiveFontBG);
  display.setCursor(5, displayLineY[1]);
  display.print("Interns:");
  display.setCursor(50, displayLineY[1]);
  display.print(intern.number);
  display.setCursor(5, displayLineY[2]);
  display.print("DipGrads:");
  display.setCursor(50, displayLineY[2]);
  display.print(dipGrad.number);
  display.setCursor(5, displayLineY[3]);
  display.print("Grads:");
  display.setCursor(50, displayLineY[3]);
  display.print(grad.number);
  display.setCursor(5, displayLineY[4]);
  display.print("Computer:");
  display.setCursor(50, displayLineY[4]);
  display.print(computerbuffer);
  display.setCursor(5, displayLineY[5]);
  display.print("Lounge:");
  display.setCursor(50, displayLineY[5]);
  display.print(loungebuffer);
}

void drawSettingsMenu() {
  display.fontColor(TS_8b_Yellow, inactiveFontBG);
  display.setCursor(5, displayLineY[0]);
  display.print("Settings");
  display.fontColor(TS_8b_White, inactiveFontBG);
  display.setCursor(20, displayLineY[2]);
  display.print("Speed:");
  display.setCursor(60, displayLineY[2]);
  display.print(psVarspeed);
  display.setCursor(3, buttonY[1]);
  display.print('<');
  display.setCursor(89, buttonY[1]);
  display.print('>');
}

void drawUpgradeMenu() {
  display.setCursor(5, displayLineY[0]);
  display.fontColor(TS_8b_Yellow, inactiveFontBG);
  display.print("Upgrade");
  display.setCursor(60, displayLineY[0]);
  display.print("Cost");
  display.fontColor(TS_8b_White, inactiveFontBG);
  display.setCursor(3, displayLineY[4]);
  display.print("Funds:");
  display.setCursor(3, displayLineY[5]);
  display.fontColor(TS_8b_Yellow, inactiveFontBG);
  display.print("Buy");
  display.fontColor(TS_8b_White, inactiveFontBG);
  display.setCursor(85, buttonY[0]);
  display.print('^');
  display.setCursor(85, buttonY[1]);
  display.print('v');
}

void drawUpgradeSelect() {
  display.clearWindow(2, 9, 84, 27);
  display.fontColor(inactiveFontColor, inactiveFontBG);
  char cost[3][8] = {"","",""};
  int costArrayMember = 0;
  for (int i = 0; i < 3; i ++) {
    costArrayMember = upgradeCostArr[menuOffset + i];
    if (upgradeCostArr[menuOffset + i]/1000000000 >1) {
      snprintf(cost[i], 8, "%iB", costArrayMember/1000000000);
    } else if (upgradeCostArr[menuOffset + i]/1000000 >1) {
      snprintf(cost[i], 8, "%iM", costArrayMember/1000000);
    } else if (upgradeCostArr[menuOffset + i]/1000 >1) {
      snprintf(cost[i], 8, "%iK", costArrayMember/1000);
    } else {
      snprintf(cost[i], 8, "%i", costArrayMember);
    }
  }
  
  if (menuSelect == 0) {display.fontColor(defaultFontColor, inactiveFontBG);}
  display.setCursor(5, displayLineY[1]);
  display.print(upgradeOptionArr[menuOffset]);
  display.setCursor(47, displayLineY[1]);
  display.print(cost[0]);
  if (menuSelect == 0) {display.fontColor(inactiveFontColor, inactiveFontBG);}
  if (menuSelect == 1) {display.fontColor(defaultFontColor, inactiveFontBG);}
  display.setCursor(5, displayLineY[2]);
  display.print(upgradeOptionArr[menuOffset+1]);
  display.setCursor(47, displayLineY[2]);
  display.print(cost[1]);
  if (menuSelect == 1) {display.fontColor(inactiveFontColor, inactiveFontBG);}
  if (menuSelect == 2) {display.fontColor(defaultFontColor, inactiveFontBG);}
  display.setCursor(5, displayLineY[3]);
  display.print(upgradeOptionArr[menuOffset+2]);
  display.setCursor(47, displayLineY[3]);
  display.print(cost[2]);
  display.fontColor(defaultFontColor, inactiveFontBG);
}
