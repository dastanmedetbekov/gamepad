#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

void parseData(String data); 

// Wi-Fi точка доступа ESP
const char* ssid = "Вифи";
const char* pass = "876543218";

// UDP
WiFiUDP udp;
const int PORT = 4210;

// IP сети
IPAddress broadcastIP;
IPAddress pcIP;
bool connected = false;

// Буфер для Serial
char buffer[32];
int bufIndex = 0;

SoftwareSerial mySerial(D6, D7); // RX, TX

// Джойстик/кнопка
int x = 0;
int y = 0;
int btn = 0;
int btn2 = 0;
int btn3 = 0;
int btn4 = 0;


// Таймер для отправки
unsigned long lastSend = 0;


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

  Serial.print("IP: ");
  Serial.println(ip);

  Serial.print("Mask: ");
  Serial.println(mask);

  // --- вычисляем broadcast ---
  for (int i = 0; i < 4; i++) {
    broadcastIP[i] = ip[i] | (~mask[i]);
  }

  Serial.print("Broadcast: ");
  Serial.println(broadcastIP);

  udp.begin(PORT);


}



void loop() {
    // --------- 1. DISCOVERY ---------
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

  // --- Чтение Serial ---
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      buffer[bufIndex] = 0;
      parseData(buffer);
      bufIndex = 0;
    }
    else {
      if (bufIndex < 31) {
        buffer[bufIndex++] = c;
    }
    }
  }

  // --- Отправка данных каждую секунду ---
  if (millis() - lastSend > 30) {

    char msg[64];
    sprintf(msg, "%d,%d,%d,%d,%d,%d", x, y, btn, btn2, btn3, btn4);

    // Отправка UDP
    udp.beginPacket(pcIP, PORT);
    udp.write(msg);
    udp.endPacket();

    // Сохраняем время отправки
    lastSend = millis();
  }
}

// --- Разбор строки из Serial ---
void parseData(String data) {
  int Comma1 = data.indexOf(',');
  int Comma2 = data.indexOf(',', Comma1 + 1);
  int Comma3 = data.indexOf(',', Comma2 + 1);
  int Comma4 = data.indexOf(',', Comma3 + 1);
  int Comma5 = data.indexOf(',', Comma4 + 1);

  if (Comma1 == -1 || Comma2 == -1) return;

  x = data.substring(0, Comma1).toInt();
  y = data.substring(Comma1 + 1, Comma2).toInt();
  btn = !data.substring(Comma2 + 1, Comma3).toInt();
  btn2 = !data.substring(Comma3 + 1, Comma4).toInt();
  btn3 = !data.substring(Comma4 + 1, Comma5).toInt();
  btn4 = !data.substring(Comma5 + 1).toInt();
}