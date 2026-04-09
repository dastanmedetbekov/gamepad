#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "wifi.h"
#include "input.h" // чтобы брать x,y,btn...

// WiFi
const char* ssid = "фиви";
const char* pass = "12345678";

// UDP
WiFiUDP udp;
const int PORT = 4210;

IPAddress broadcastIP;
IPAddress pcIP;
bool connected = false;

unsigned long lastSend = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  IPAddress ip = WiFi.localIP();
  IPAddress mask = WiFi.subnetMask();

  for (int i = 0; i < 4; i++) {
    broadcastIP[i] = ip[i] | (~mask[i]);
  }

  udp.begin(PORT);
}

void handleWiFi() {

  // --- DISCOVERY ---
  if (!connected) {
    udp.beginPacket(broadcastIP, PORT);
    udp.write("HELLO");
    udp.endPacket();

    delay(200);

    int packetSize = udp.parsePacket();
    if (packetSize) {
      char buf[16];
      int len = udp.read(buf, 15);
      buf[len] = 0;

      if (strcmp(buf, "OK") == 0) {
        pcIP = udp.remoteIP();
        connected = true;
        Serial.println("Connected to PC!");
      }
    }
    return;
  }

  // --- ОТПРАВКА ---
  if (millis() - lastSend > 30) {

    char msg[64];
    sprintf(msg, "%d,%d,%d,%d,%d,%d", x, y, btn, btn2, btn3, btn4);

    udp.beginPacket(pcIP, PORT);
    udp.write(msg);
    udp.endPacket();

    lastSend = millis();
  }
}