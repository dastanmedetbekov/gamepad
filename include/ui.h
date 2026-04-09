#ifndef UI_H
#define UI_H

enum Screen {
  MAIN_SCREEN,
  WIFI_MENU
};

void initDisplay();
void drawUI();
void setScreen(Screen s);

#endif