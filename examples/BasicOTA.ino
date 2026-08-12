#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

const char* ssid = "ESP32-OTA";
const char* password = "12345678";

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);

  IPAddress IP(10, 0, 0, 9);
  IPAddress gateway(10, 0, 0, 9);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("================================");
  Serial.println("OTAWIFIAsyncBar");
  Serial.println("================================");

  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  ota.begin(&server);

  server.begin();

  Serial.println("OTA server START");
  Serial.println("Open: http://10.0.0.9/update");
}

void loop()
{
  static int ostatniPostep = -1;

  if (OTA_aktywny && OTA_postep != ostatniPostep)
  {
    ostatniPostep = OTA_postep;

    Serial.print("OTA: ");
    Serial.print(OTA_postep);
    Serial.println("%");
  }
}