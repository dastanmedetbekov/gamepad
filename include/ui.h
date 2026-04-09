#ifndef UI_H
#define UI_H

enum Screen {
  MAIN_SCREEN,
  MENU_SCREEN
};

void initUI();
void drawUI();
void setScreen(Screen s);

#endif