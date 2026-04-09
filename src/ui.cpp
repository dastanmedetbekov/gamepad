#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Screen currentScreen = MAIN_SCREEN;

void initDisplay() {
  Wire.begin(D2, D1);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void setScreen(Screen s) {
  currentScreen = s;
}

void drawMain() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("MAIN"));
  display.display();
}

void drawMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("> WiFi"));
  display.println(F("  Back"));
  display.display();
}

void drawUI() {
  switch (currentScreen) {
    case MAIN_SCREEN: drawMain(); break;
    case WIFI_MENU: drawMenu(); break;
  }
}