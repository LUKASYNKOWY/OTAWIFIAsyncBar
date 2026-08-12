# OTAWIFIAsyncBar

Asynchronous WiFi OTA firmware update library for ESP32.

OTAWIFIAsyncBar provides a simple web interface for uploading firmware to an ESP32 over WiFi.

The library is based on `ESPAsyncWebServer` and `AsyncTCP`. Because the web server is asynchronous, the application does not require `server.handleClient()` in the main `loop()`.

## Features

* Asynchronous OTA firmware update
* ESP32 support
* WiFi Access Point support
* Firmware upload using `.bin` files
* Web-based firmware update interface
* Upload progress bar
* OTA progress available to the application
* OTA activity status available to the application
* OTA error status available to the application
* Serial progress information
* Automatic ESP32 restart after successful update
* No `server.handleClient()` required
* Compatible with other asynchronous HTTP endpoints
* Customizable HTML interface

## Requirements

### Hardware

* ESP32

### Software

* Arduino IDE
* ESP32 Arduino Core
* AsyncTCP
* ESP Async WebServer

## Installation

### Arduino IDE

Download or clone this repository and place the library folder in:

```text
Arduino/libraries/OTAWIFIAsyncBar
```

The final structure should be:

```text
Arduino/
└── libraries/
    └── OTAWIFIAsyncBar/
        ├── src/
        │   ├── OTAWIFIAsyncBar.cpp
        │   ├── OTAWIFIAsyncBar.h
        │   └── OTAWIFIAsyncBarPage.h
        ├── examples/
        │   ├── BasicOTA/
        │   └── OLED_OTA/
        ├── library.properties
        ├── keywords.txt
        ├── README.md
        ├── LICENSE
        └── .gitignore
```

After installing the library, restart Arduino IDE.

## Basic Usage

Include the required libraries:

```cpp
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>
```

Create the asynchronous web server and OTA object:

```cpp
AsyncWebServer server(80);
OTAWIFIAsyncBar ota;
```

Configure WiFi and initialize OTA:

```cpp
void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-OTA", "12345678");

    ota.begin(&server);

    server.begin();

    Serial.println("OTA server started");
}
```

The main `loop()` does not require:

```cpp
server.handleClient();
```

The asynchronous server works without polling:

```cpp
void loop()
{
}
```

## OTA Update

After starting the ESP32:

1. Connect to the WiFi network created by the ESP32.
2. Open the ESP32 IP address in a web browser.
3. Open the OTA update page at `/update`.
4. Select the compiled `.bin` firmware file.
5. Start the update.
6. Wait until the upload is complete.
7. The ESP32 automatically restarts after a successful update.

The firmware file can be generated from Arduino IDE using:

```text
Sketch -> Export Compiled Binary
```

The generated `.bin` file can then be uploaded through the OTA interface.

## OTA Progress

The library provides OTA status information to the main application.

Use `isActive()` to check whether an OTA upload is currently active.

Use `getProgress()` to read the current upload progress.

Example:

```cpp
if (ota.isActive())
{
    int progress = ota.getProgress();

    Serial.print("OTA: ");
    Serial.print(progress);
    Serial.println("%");
}
```

`getProgress()` returns the current upload progress in percent:

```text
0 - 100
```

## OTA Error Status

The library also provides an OTA error status through `hasError()`.

Example:

```cpp
if (ota.hasError())
{
    Serial.println("OTA update failed");
}
```

This allows the main application to react to an OTA error.

## Example with OLED

OTAWIFIAsyncBar does not require an OLED display.

The OTA progress can optionally be displayed on an OLED or another display by the user's application.

Example:

```cpp
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
```

The OLED library and display initialization are not part of OTAWIFIAsyncBar.

The display is completely controlled by the user's application.

A complete OLED example is included in:

```text
examples/OLED_OTA
```

## Access Point Example

A basic WiFi Access Point configuration:

```cpp
WiFi.mode(WIFI_AP);

WiFi.softAP(
    "ESP32-OTA",
    "12345678"
);

Serial.println(WiFi.softAPIP());
```

Then initialize the OTA server:

```cpp
ota.begin(&server);
server.begin();
```

The IP address printed by `WiFi.softAPIP()` can be entered into a web browser.

The OTA interface is available at:

```text
/update
```

## External WebServer

OTAWIFIAsyncBar does not create its own `AsyncWebServer`.

The application provides an existing `AsyncWebServer` object:

```cpp
AsyncWebServer server(80);

OTAWIFIAsyncBar ota;
```

Initialize the OTA functionality using:

```cpp
void setup()
{
    ota.begin(&server);

    server.begin();
}
```

This allows the OTA functionality to coexist with other asynchronous HTTP endpoints.

For example:

```cpp
server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
{
    request->send(200, "text/plain", "ESP32 OK");
});
```

The application can therefore use both the OTA interface and its own web endpoints.

## Custom Web Interface

The default OTA web interface is stored in:

```text
src/OTAWIFIAsyncBarPage.h
```

The HTML page can be modified without changing the OTA upload mechanism.

The interface communicates with the OTA server using:

```text
GET  /update
POST /update
```

The page can therefore be customized for a particular project.

The web page is embedded directly into the library and does not require SPIFFS, LittleFS or an external web server.

## How the OTA Process Works

When the user opens:

```text
/update
```

the library sends the embedded HTML interface to the browser.

When a `.bin` file is selected and uploaded, the browser sends the firmware to:

```text
POST /update
```

The library receives the firmware in chunks and writes the data using the ESP32 `Update` API.

During the upload:

* `isActive()` reports that an OTA update is active.
* `getProgress()` reports the current upload progress.
* Serial output reports upload progress.
* `hasError()` reports whether an OTA error occurred.

After the complete firmware has been received, the library finalizes the update.

If the update succeeds, the ESP32 automatically restarts.

## Examples

The repository contains two examples:

### BasicOTA

A minimal example demonstrating:

* ESP32 WiFi Access Point
* Asynchronous web server
* OTA firmware update
* Upload progress

Location:

```text
examples/BasicOTA
```

### OLED_OTA

An example demonstrating:

* ESP32 WiFi Access Point
* Asynchronous web server
* OTA firmware update
* Upload progress
* Displaying OTA progress on an OLED

Location:

```text
examples/OLED_OTA
```

## Dependencies

OTAWIFIAsyncBar requires:

```text
AsyncTCP
ESP Async WebServer
```

These libraries must be installed separately.

OTAWIFIAsyncBar also requires an ESP32 board using the Arduino framework.

## Important

The OTA update should only be performed while the ESP32 has a stable power supply.

Do not disconnect the ESP32 or interrupt the firmware upload during the update.

An interrupted firmware update can result in an unusable application image.

Make sure that the firmware being uploaded is compatible with the target ESP32.

## API Reference

### Constructor

```cpp
OTAWIFIAsyncBar ota;
```

Creates an OTAWIFIAsyncBar object.

### begin()

```cpp
ota.begin(&server);
```

Initializes the OTA routes using an existing `AsyncWebServer`.

The application must call:

```cpp
server.begin();
```

after initializing the OTA server.

### getProgress()

```cpp
int progress = ota.getProgress();
```

Returns the current OTA upload progress in percent.

Range:

```text
0 - 100
```

### isActive()

```cpp
bool active = ota.isActive();
```

Returns `true` while an OTA firmware upload is active.

### hasError()

```cpp
bool error = ota.hasError();
```

Returns `true` if an OTA error has occurred.

## Complete Basic Example

```cpp
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-OTA", "12345678");

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    ota.begin(&server);

    server.begin();

    Serial.println("OTA server started");
}

void loop()
{
    if (ota.isActive())
    {
        int progress = ota.getProgress();

        Serial.print("OTA: ");
        Serial.print(progress);
        Serial.println("%");
    }
}
```

No `server.handleClient()` call is required.

## Complete OLED Example

The following example shows how the OTA progress can be displayed by the application.

```cpp
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

Adafruit_SH1106G OLED(128, 64);

void setup()
{
    Serial.begin(115200);

    OLED.begin(0x3C, true);
    OLED.clearDisplay();
    OLED.setTextColor(SH110X_WHITE);
    OLED.setTextSize(1);
    OLED.setCursor(0, 0);
    OLED.println("OTA READY");
    OLED.display();

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-OTA", "12345678");

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    ota.begin(&server);

    server.begin();

    Serial.println("OTA server started");
}

void loop()
{
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

        OLED.drawRect(0, 40, 128, 12, SH110X_WHITE);

        int width = map(progress, 0, 100, 0, 124);

        if (width > 0)
        {
            OLED.fillRect(2, 42, width, 8, SH110X_WHITE);
        }

        OLED.display();
    }
}
```

The OLED is optional. The OTA functionality works without it.

## License

This project is licensed under the MIT License.

See the `LICENSE` file for the complete license text.

## Author

Zaju

## Repository

GitHub:

https://github.com/LUKASYNKOWY/OTAWIFIAsyncBar
