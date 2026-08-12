# OTAWIFIAsyncBar

Asynchronous WiFi OTA firmware update library for ESP32.

OTAWIFIAsyncBar provides a simple web interface for uploading firmware to an ESP32 over WiFi.

The library is based on ESPAsyncWebServer and AsyncTCP and does not require `server.handleClient()` in the main `loop()`.

## Features

- Asynchronous OTA firmware update
- ESP32 support
- WiFi Access Point support
- Firmware upload using `.bin` files
- Web-based firmware update interface
- Upload progress bar
- OTA progress available to the application
- OTA activity status available to the application
- Serial progress information
- Automatic ESP32 restart after successful update
- No `server.handleClient()` required
- Compatible with asynchronous ESP32 web applications
- Customizable HTML interface

## Requirements

### Hardware

- ESP32

### Software

- Arduino IDE
- ESP32 Arduino Core
- AsyncTCP
- ESP Async WebServer

## Installation

### Arduino IDE

Download or clone this repository and place the library folder in:

    Arduino/libraries/OTAWIFIAsyncBar

The final structure should be:

    Arduino/
    └── libraries/
        └── OTAWIFIAsyncBar/
            ├── src/
            │   ├── OTAWIFIAsyncBar.cpp
            │   ├── OTAWIFIAsyncBar.h
            │   └── OTAWIFIAsyncBarPage.h
            ├── examples/
            ├── library.properties
            ├── keywords.txt
            ├── README.md
            ├── LICENSE
            └── .gitignore

After installing the library, restart Arduino IDE.

## Basic usage

Include the required libraries:

```cpp
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

Create the asynchronous web server and OTA object:

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

Configure WiFi and initialize OTA:

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-OTA", "12345678");

    ota.begin(&server);

    server.begin();

    Serial.println("OTA server started");
}

The main loop() does not require:

server.handleClient();

The asynchronous server works without polling:

void loop()
{
}
OTA update

After starting the ESP32:

Connect to the WiFi network created by the ESP32.
Open the ESP32 IP address in a web browser.
Open the OTA update page.
Select the compiled .bin firmware file.
Start the update.
Wait until the upload is complete.
The ESP32 automatically restarts after a successful update.

The firmware file can be generated from Arduino IDE using:

Sketch -> Export Compiled Binary

The generated .bin file can then be uploaded through the OTA interface.

OTA progress

The library provides OTA status information to the main application.

Example:

if (ota.isActive())
{
    int progress = ota.getProgress();

    Serial.print("OTA: ");
    Serial.print(progress);
    Serial.println("%");
}

getProgress() returns the current upload progress in percent:

0 - 100
Example with OLED

The OTA progress can also be displayed on an OLED or another display.

Example:

if (ota.isActive())
{
    int progress = ota.getProgress();

    OLED.clearDisplay();

    OLED.setCursor(0, 0);
    OLED.println("OTA UPDATE");

    OLED.setCursor(0, 20);
    OLED.print("Progress: ");
    OLED.print(progress);
    OLED.println("%");

    OLED.display();
}

The library does not require an OLED.

The display is handled by the user's application.

Custom web interface

The OTA web interface is stored in:

src/OTAWIFIAsyncBarPage.h

The HTML page can be modified without changing the OTA upload mechanism.

The page communicates with the OTA server using:

GET  /update
POST /update

The HTML interface can therefore be customized for a particular project.

Access Point example

A basic Access Point configuration:

WiFi.mode(WIFI_AP);

WiFi.softAP(
    "ESP32-OTA",
    "12345678"
);

Serial.println(WiFi.softAPIP());

Then initialize the OTA server:

ota.begin(&server);
server.begin();
External WebServer

OTAWIFIAsyncBar does not create its own web server.

The application provides an existing AsyncWebServer object:

AsyncWebServer server(80);

OTAWIFIAsyncBar ota;

void setup()
{
    ota.begin(&server);
    server.begin();
}

This allows the OTA functionality to coexist with other asynchronous HTTP endpoints.

For example:

server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
{
    request->send(200, "text/plain", "ESP32 OK");
});
Important

The OTA update should only be performed while the ESP32 has a stable power supply.

Do not disconnect the ESP32 or interrupt the firmware upload during the update.

An interrupted firmware update can result in an unusable application image.

Dependencies

OTAWIFIAsyncBar requires:

AsyncTCP
ESP Async WebServer

These libraries must be installed separately.

Examples

The repository contains examples demonstrating:

Basic OTA operation
OTA operation with an OLED display

See the examples directory.

License

This project is licensed under the MIT License.

See the LICENSE file for the complete license text.