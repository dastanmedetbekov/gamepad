#include <Arduino.h>
#include <SoftwareSerial.h>
#include "input.h"

SoftwareSerial mySerial(D6, D7);

// данные
int x = 0;
int y = 0;
int btn = 0;
int btn2 = 0;
int btn3 = 0;
int btn4 = 0;

// буфер
char buffer[32];
int bufIndex = 0;

void parseData(char* data);

void initInput() {
  mySerial.begin(9600);
}

void handleInput() {

  while (mySerial.available()) {
    char c = mySerial.read();

    if (c == '\n') {
      buffer[bufIndex] = 0;
      parseData(buffer);
      bufIndex = 0;
    } else {
      if (bufIndex < 31) {
        buffer[bufIndex++] = c;
      }
    }
  }
}

// --- ПАРСИНГ БЕЗ String ---
void parseData(char* data) {
  sscanf(data, "%d,%d,%d,%d,%d,%d",
         &x, &y, &btn, &btn2, &btn3, &btn4);

  btn  = !btn;
  btn2 = !btn2;
  btn3 = !btn3;
  btn4 = !btn4;
}