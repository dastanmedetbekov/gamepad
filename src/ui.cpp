#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ui.h"
#include "input.h"
#include "wifi.h"

// экран
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// состояние
Screen currentScreen = MAIN_SCREEN;
int menuIndex = 0;

// ===== INIT =====
void initUI() {
  Wire.begin(D2, D1);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

// ===== API =====
void setScreen(Screen s) {
  currentScreen = s;
}

// ===== ЭКРАНЫ =====

void drawMain() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println(F("GAMEPAD"));

  display.print(F("X: "));
  display.println(x);

  display.print(F("Y: "));
  display.println(y);

  display.print(F("WiFi: "));
  display.println(connected ? F("OK") : F("..."));

  display.println(F("\nBTN -> Menu"));

  display.display();
}

void drawMenu() {
  display.clearDisplay();

  const char* items[3] = {
    "WiFi",
    "Hack Mode",
    "Back"
  };

  for (int i = 0; i < 3; i++) {
    if (i == menuIndex) display.print("> ");
    else display.print("  ");

    display.println(items[i]);
  }

  display.display();
}

// ===== ГЛАВНАЯ ОТРИСОВКА =====

void drawUI() {

  // --- управление меню ---
  static unsigned long lastMove = 0;

  if (millis() - lastMove > 200) {

    if (y > 800) {         // вниз
      menuIndex++;
      if (menuIndex > 2) menuIndex = 0;
      lastMove = millis();
    }

    if (y < 200) {         // вверх
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2;
      lastMove = millis();
    }
  }

  // --- выбор ---
  if (btn) {
    if (currentScreen == MAIN_SCREEN) {
      setScreen(MENU_SCREEN);
      delay(200);
    }
    else if (currentScreen == MENU_SCREEN) {
      if (menuIndex == 2) {
        setScreen(MAIN_SCREEN);
      }
      delay(200);
    }
  }

  // --- отрисовка ---
  switch (currentScreen) {
    case MAIN_SCREEN: drawMain(); break;
    case MENU_SCREEN: drawMenu(); break;
  }
}