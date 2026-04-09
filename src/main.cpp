#include <Arduino.h>
#include "wifi.h"
#include "input.h"

void setup() {
  Serial.begin(9600);

  initInput();
  initWiFi();
}

void loop() {
  handleWiFi();   // discovery + отправка
  handleInput();  // чтение джойстика
}