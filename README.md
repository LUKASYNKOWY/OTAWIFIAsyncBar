# OTAWIFIAsyncBar

Asynchronous WiFi OTA firmware update library for ESP32.

OTAWIFIAsyncBar provides a simple web interface that allows firmware
updates of an ESP32 device over WiFi.

The library uses ESPAsyncWebServer and AsyncTCP, so the web server
does not require `server.handleClient()` in the main `loop()`.

## Features

- ESP32 WiFi Access Point OTA
- Asynchronous web server
- Firmware upload using `.bin` files
- Upload progress bar
- OTA progress available to the main application
- Serial progress information
- Automatic ESP32 restart after successful update
- Simple web interface
- No blocking `server.handleClient()` loop

## Requirements

- ESP32
- Arduino framework
- AsyncTCP
- ESPAsyncWebServer

## Basic usage

```cpp
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

void setup()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-OTA", "12345678");

    ota.begin(&server);

    server.begin();
}

void loop()
{
}