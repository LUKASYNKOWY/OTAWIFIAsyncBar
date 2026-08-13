#include "OTAWIFIAsyncBar.h"
#include "OTAWIFIAsyncBarPage.h"

volatile int OTAWIFIAsyncBar_progress = 0;
volatile bool OTAWIFIAsyncBar_active = false;
volatile bool OTAWIFIAsyncBar_error = false;

volatile OTAError OTAWIFIAsyncBar_errorCode =
    OTAError::NONE;

OTAWIFIAsyncBar::OTAWIFIAsyncBar()
{
    _server = nullptr;
}

void OTAWIFIAsyncBar::begin(
    AsyncWebServer *server
)
{
    _server = server;

    OTAWIFIAsyncBar_progress = 0;
    OTAWIFIAsyncBar_active = false;
    OTAWIFIAsyncBar_error = false;
    OTAWIFIAsyncBar_errorCode =
        OTAError::NONE;

    _server->on(
        "/update",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(
                200,
                "text/html",
                OTAWIFIAsyncBarPage
            );
        }
    );

    _server->on(
        "/update",
        HTTP_POST,

        [](AsyncWebServerRequest *request)
        {
            if (OTAWIFIAsyncBar_error)
            {
                request->send(
                    200,
                    "text/plain",
                    "FAIL"
                );

                return;
            }

            if (Update.hasError())
            {
                OTAWIFIAsyncBar_error = true;
                OTAWIFIAsyncBar_active = false;

                if (
                    OTAWIFIAsyncBar_errorCode ==
                    OTAError::NONE
                )
                {
                    OTAWIFIAsyncBar_errorCode =
                        OTAError::UNKNOWN;
                }

                request->send(
                    200,
                    "text/plain",
                    "FAIL"
                );

                Serial.println();
                Serial.println(
                    "========== OTA ERROR =========="
                );

                Serial.print(
                    "Error code: "
                );

                Serial.println(
                    (int)OTAWIFIAsyncBar_errorCode
                );

                return;
            }

            OTAWIFIAsyncBar_progress = 100;
            OTAWIFIAsyncBar_active = false;
            OTAWIFIAsyncBar_error = false;
            OTAWIFIAsyncBar_errorCode =
                OTAError::NONE;

            AsyncWebServerResponse *response =
                request->beginResponse(
                    200,
                    "text/plain",
                    "OK"
                );

            response->addHeader(
                "Connection",
                "close"
            );

            request->send(response);

            Serial.println();
            Serial.println(
                "========== OTA SUCCESS =========="
            );

            Serial.println(
                "Firmware accepted."
            );

            delay(500);

            ESP.restart();
        },

        [](AsyncWebServerRequest *request,
           String filename,
           size_t index,
           uint8_t *data,
           size_t len,
           bool final)
        {
            if (index == 0)
            {
                OTAWIFIAsyncBar_progress = 0;
                OTAWIFIAsyncBar_active = true;
                OTAWIFIAsyncBar_error = false;
                OTAWIFIAsyncBar_errorCode =
                    OTAError::NONE;

                request->onDisconnect(
                    [](void)
                    {
                        if (
                            OTAWIFIAsyncBar_active
                        )
                        {
                            OTAWIFIAsyncBar_error =
                                true;

                            OTAWIFIAsyncBar_active =
                                false;

                            OTAWIFIAsyncBar_errorCode =
                                OTAError::DISCONNECTED;

                            OTAWIFIAsyncBar_progress =
                                0;

                            Update.abort();

                            Serial.println();
                            Serial.println(
                                "========== OTA DISCONNECTED =========="
                            );

                            Serial.println(
                                "OTA upload interrupted."
                            );
                        }
                    }
                );

                Serial.println();
                Serial.println(
                    "========== OTA START =========="
                );

                Serial.print(
                    "File: "
                );

                Serial.println(
                    filename
                );

                Serial.print(
                    "HTTP size: "
                );

                Serial.println(
                    request->contentLength()
                );

                String lowerFilename =
                    filename;

                lowerFilename.toLowerCase();

                if (
                    !lowerFilename.endsWith(
                        ".bin"
                    )
                )
                {
                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FILE;

                    Serial.println(
                        "OTA ERROR: Invalid file"
                    );

                    return;
                }

                size_t totalSize =
                    request->contentLength();

                if (
                    totalSize < 24
                )
                {
                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FIRMWARE;

                    Serial.println(
                        "OTA ERROR: File too small"
                    );

                    return;
                }

                if (
                    len < 2
                )
                {
                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FIRMWARE;

                    Serial.println(
                        "OTA ERROR: Invalid firmware header"
                    );

                    return;
                }

                uint8_t magic =
                    data[0];

                uint8_t segmentCount =
                    data[1];

                if (
                    magic != 0xE9
                )
                {
                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FIRMWARE;

                    Serial.println(
                        "OTA ERROR: Not an ESP32 firmware"
                    );

                    Serial.print(
                        "Magic: 0x"
                    );

                    Serial.println(
                        magic,
                        HEX
                    );

                    return;
                }

                if (
                    segmentCount == 0 ||
                    segmentCount > 16
                )
                {
                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FIRMWARE;

                    Serial.println(
                        "OTA ERROR: Invalid segment count"
                    );

                    Serial.print(
                        "Segments: "
                    );

                    Serial.println(
                        segmentCount
                    );

                    return;
                }

                if (
                    !Update.begin(
                        UPDATE_SIZE_UNKNOWN
                    )
                )
                {
                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::BEGIN_FAILED;

                    Serial.println(
                        "OTA ERROR: Update.begin()"
                    );

                    return;
                }

                Serial.println(
                    "Firmware header OK"
                );

                Serial.print(
                    "Segments: "
                );

                Serial.println(
                    segmentCount
                );

                Serial.println(
                    "Update.begin() OK"
                );
            }

            if (
                len > 0 &&
                !OTAWIFIAsyncBar_error
            )
            {
                size_t written =
                    Update.write(
                        data,
                        len
                    );

                if (
                    written != len
                )
                {
                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::WRITE_FAILED;

                    Update.abort();

                    Serial.println(
                        "OTA ERROR: Update.write()"
                    );

                    return;
                }

                size_t uploaded =
                    index + len;

                size_t total =
                    request->contentLength();

                if (
                    total > 0
                )
                {
                    int progress =
                        (uploaded * 100) /
                        total;

                    if (
                        progress > 100
                    )
                    {
                        progress = 100;
                    }

                    OTAWIFIAsyncBar_progress =
                        progress;

                    Serial.print(
                        "OTA PROGRESS: "
                    );

                    Serial.print(
                        OTAWIFIAsyncBar_progress
                    );

                    Serial.print(
                        "% | "
                    );

                    Serial.print(
                        uploaded
                    );

                    Serial.print(
                        "/"
                    );

                    Serial.println(
                        total
                    );
                }
            }

            if (final)
            {
                if (
                    OTAWIFIAsyncBar_error
                )
                {
                    return;
                }

                Serial.println();
                Serial.println(
                    "========== OTA END =========="
                );

                Serial.print(
                    "Uploaded: "
                );

                Serial.println(
                    index + len
                );

                if (
                    Update.end(true)
                )
                {
                    OTAWIFIAsyncBar_progress =
                        100;

                    Serial.println(
                        "Update.end() OK"
                    );

                    Serial.print(
                        "Update Success: "
                    );

                    Serial.println(
                        index + len
                    );
                }
                else
                {
                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_active =
                        false;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::END_FAILED;

                    Serial.println(
                        "OTA ERROR: Update.end()"
                    );
                }
            }
        }
    );
}

void OTAWIFIAsyncBar::loop()
{
}

int OTAWIFIAsyncBar::getProgress()
{
    return OTAWIFIAsyncBar_progress;
}

bool OTAWIFIAsyncBar::isActive()
{
    return OTAWIFIAsyncBar_active;
}

bool OTAWIFIAsyncBar::hasError()
{
    return OTAWIFIAsyncBar_error;
}

OTAError OTAWIFIAsyncBar::getError()
{
    return OTAWIFIAsyncBar_errorCode;
}

String OTAWIFIAsyncBar::getErrorString()
{
    switch (
        OTAWIFIAsyncBar_errorCode
    )
    {
        case OTAError::NONE:
            return "No error";

        case OTAError::BEGIN_FAILED:
            return "Start failed";

        case OTAError::WRITE_FAILED:
            return "Write failed";

        case OTAError::END_FAILED:
            return "Finish failed";

        case OTAError::DISCONNECTED:
            return "Connection lost";

        case OTAError::INVALID_FILE:
            return "Invalid file";

        case OTAError::INVALID_FIRMWARE:
            return "Invalid firmware";

        case OTAError::UNKNOWN:
            return "Unknown error";

        default:
            return "Unknown error";
    }
}

void OTAWIFIAsyncBar::clearError()
{
    OTAWIFIAsyncBar_progress = 0;

    OTAWIFIAsyncBar_active = false;

    OTAWIFIAsyncBar_error = false;

    OTAWIFIAsyncBar_errorCode =
        OTAError::NONE;
}
