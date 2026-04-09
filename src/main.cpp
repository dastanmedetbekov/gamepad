#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

void parseData(char* data);

const char* ssid = "Вифи";
const char* pass = "876543218";

WiFiUDP udp;
const int PORT = 4210;

IPAddress broadcastIP;
IPAddress pcIP;
bool connected = false;

char buffer[32];
int bufIndex = 0;

SoftwareSerial mySerial(D6, D7);

int x = 0, y = 0;
int btn = 0, btn2 = 0, btn3 = 0, btn4 = 0;

unsigned long lastSend = 0;
unsigned long lastDiscovery = 0;
unsigned long lastPacketTime = 0;
const unsigned long KEEPALIVE_TIMEOUT = 5000;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

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
  Serial.print("IP: "); Serial.println(ip);
  Serial.print("Mask: "); Serial.println(mask);

  for (int i = 0; i < 4; i++) {
    broadcastIP[i] = ip[i] | (uint8_t)(~mask[i]);
  }
  Serial.print("Broadcast: "); Serial.println(broadcastIP);

  udp.begin(PORT);
}

void loop() {
  if (!connected) {
    if (millis() - lastDiscovery > 200) {
      lastDiscovery = millis();
      udp.beginPacket(broadcastIP, PORT);
      udp.write("HELLO");
      udp.endPacket();
    }

    int packetSize = udp.parsePacket();
    if (packetSize) {
      char buf[16];
      int len = udp.read(buf, 15);
      buf[len] = '\0';
      if (strcmp(buf, "OK") == 0) {
        pcIP = udp.remoteIP();
        connected = true;
        lastPacketTime = millis();
        Serial.println("Connected to PC!");
      }
    }
    return;
  }

  if (millis() - lastPacketTime > KEEPALIVE_TIMEOUT) {
    connected = false;
    Serial.println("PC timeout, reconnecting...");
    return;
  }

  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    char buf[16];
    int len = udp.read(buf, 15);
    buf[len] = '\0';
    lastPacketTime = millis();
  }

  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      buffer[bufIndex] = '\0';
      parseData(buffer);
      bufIndex = 0;
    } else {
      if (bufIndex < 31) buffer[bufIndex++] = c;
    }
  }

  if (millis() - lastSend > 30) {
    lastSend = millis();
    char msg[64];
    sprintf(msg, "%d,%d,%d,%d,%d,%d", x, y, btn, btn2, btn3, btn4);
    udp.beginPacket(pcIP, PORT);
    udp.write(msg);
    udp.endPacket();
  }
}

void parseData(char* data) {
  char* token = strtok(data, ",");
  if (!token) return; x = atoi(token);

  token = strtok(NULL, ",");
  if (!token) return; y = atoi(token);

  token = strtok(NULL, ",");
  if (!token) return; btn = !atoi(token);

  token = strtok(NULL, ",");
  if (!token) return; btn2 = !atoi(token);

  token = strtok(NULL, ",");
  if (!token) return; btn3 = !atoi(token);

  token = strtok(NULL, ",");
  if (!token) return; btn4 = !atoi(token);
}
