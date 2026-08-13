#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <OTAWIFIAsyncBar.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "ESP32-OTA";
const char* password = "12345678";

// =====================================================
// SERVER / OTA
// =====================================================

AsyncWebServer server(80);
OTAWIFIAsyncBar ota;

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("================================");
    Serial.println("       OTAWIFIAsyncBar");
    Serial.println("       Full Example");
    Serial.println("================================");

    // -------------------------------------------------
    // WIFI ACCESS POINT
    // -------------------------------------------------

    WiFi.mode(WIFI_AP);

    WiFi.softAP(
        ssid,
        password
    );

    Serial.println();
    Serial.println("WiFi AP started");

    Serial.print("SSID: ");
    Serial.println(ssid);

    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    // -------------------------------------------------
    // CUSTOM HTTP ENDPOINT
    // -------------------------------------------------

    server.on(
        "/status",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            request->send(
                200,
                "text/plain",
                "ESP32 is running"
            );
        }
    );

    // -------------------------------------------------
    // OTA
    // -------------------------------------------------

    ota.begin(&server);

    // -------------------------------------------------
    // START SERVER
    // -------------------------------------------------

    server.begin();

    Serial.println();
    Serial.println("OTA server started");
    Serial.println("Open:");
    Serial.print("http://");
    Serial.print(WiFi.softAPIP());
    Serial.println("/update");

    Serial.println();
    Serial.println("System ready.");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    // IMPORTANT:
    // Required for OTA timeout detection.

    ota.loop();

    // -------------------------------------------------
    // OTA ACTIVE
    // -------------------------------------------------

    if (ota.isActive())
    {
        static int lastProgress = -1;

        int progress = ota.getProgress();

        if (progress != lastProgress)
        {
            lastProgress = progress;

            Serial.print("OTA progress: ");
            Serial.print(progress);
            Serial.println("%");
        }
    }

    // -------------------------------------------------
    // OTA ERROR
    // -------------------------------------------------

    if (ota.hasError())
    {
        Serial.println();
        Serial.println("========== OTA ERROR ==========");

        Serial.print("Error code: ");

        OTAError error = ota.getError();

        switch (error)
        {
            case OTAError::NONE:
                Serial.println("NONE");
                break;

            case OTAError::BEGIN_FAILED:
                Serial.println("BEGIN_FAILED");
                break;

            case OTAError::WRITE_FAILED:
                Serial.println("WRITE_FAILED");
                break;

            case OTAError::END_FAILED:
                Serial.println("END_FAILED");
                break;

            case OTAError::TIMEOUT:
                Serial.println("TIMEOUT");
                break;

            case OTAError::INVALID_FILE:
                Serial.println("INVALID_FILE");
                break;

            case OTAError::UNKNOWN:
                Serial.println("UNKNOWN");
                break;
        }

        Serial.print("Description: ");
        Serial.println(
            ota.getErrorString()
        );

        Serial.println("===============================");
        Serial.println();

        // Clear error after handling it.
        ota.clearError();
    }

    delay(10);
}