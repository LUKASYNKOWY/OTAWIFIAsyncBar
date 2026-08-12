#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_RESET -1

Adafruit_SH1106G OLED(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "ESP32-OTA";
const char* password = "12345678";

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

void pokazOLED(const char* tekst)
{
  OLED.clearDisplay();

  OLED.setTextSize(1);
  OLED.setTextColor(SH110X_WHITE);

  OLED.setCursor(0, 0);
  OLED.println("OTAWIFIAsyncBar");

  OLED.setCursor(0, 20);
  OLED.println(tekst);

  OLED.display();
}

void pokazPostep()
{
  int postep = OTA_postep;

  if (postep < 0)
    postep = 0;

  if (postep > 100)
    postep = 100;

  OLED.clearDisplay();

  OLED.setTextSize(1);
  OLED.setTextColor(SH110X_WHITE);

  OLED.setCursor(0, 0);
  OLED.println("OTA UPDATE");

  OLED.setCursor(0, 20);
  OLED.print("Postep: ");
  OLED.print(postep);
  OLED.println("%");

  OLED.drawRect(0, 40, 128, 12, SH110X_WHITE);

  int szerokosc = map(postep, 0, 100, 0, 124);

  if (szerokosc > 0)
  {
    OLED.fillRect(2, 42, szerokosc, 8, SH110X_WHITE);
  }

  OLED.display();
}

void setup()
{
  Serial.begin(115200);

  Wire.begin();

  if (!OLED.begin(0x3C, true))
  {
    Serial.println("Blad OLED!");
  }
  else
  {
    pokazOLED("Uruchamianie...");
  }

  WiFi.mode(WIFI_AP);

  IPAddress IP(10, 0, 0, 9);
  IPAddress gateway(10, 0, 0, 9);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("================================");
  Serial.println("OTAWIFIAsyncBar + OLED");
  Serial.println("================================");

  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  ota.begin(&server);

  server.begin();

  pokazOLED("Gotowy do OTA");

  Serial.println("OTA server START");
  Serial.println("Open: http://10.0.0.9/update");
}

void loop()
{
  static int ostatniPostep = -1;
  static bool poprzednioAktywny = false;

  if (OTA_aktywny)
  {
    if (!poprzednioAktywny)
    {
      poprzednioAktywny = true;
      ostatniPostep = -1;

      Serial.println("OTA START");

      pokazOLED("Aktualizowanie...");
    }

    if (OTA_postep != ostatniPostep)
    {
      ostatniPostep = OTA_postep;

      Serial.print("OTA: ");
      Serial.print(OTA_postep);
      Serial.println("%");

      pokazPostep();
    }
  }
  else
  {
    if (poprzednioAktywny)
    {
      poprzednioAktywny = false;

      Serial.println("OTA END");

      pokazOLED("Gotowy do OTA");
    }
  }
}