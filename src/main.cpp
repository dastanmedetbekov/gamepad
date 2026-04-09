#include <Arduino.h>
#include "wifi.h"
#include "input.h"
#include "ui.h"

void setup() {
  Serial.begin(9600);

  initInput();
  initWiFi();
  initUI();
}

void loop() {
  handleWiFi();   // discovery + отправка
  handleInput();  // чтение джойстика
  drawUI();
}