#include "OTAWIFIAsyncBar.h"
#include "OTAWIFIAsyncBarPage.h"


// =====================================================
// GLOBAL STATE
// =====================================================

volatile int OTAWIFIAsyncBar_progress = 0;

volatile bool OTAWIFIAsyncBar_active = false;

volatile bool OTAWIFIAsyncBar_error = false;

volatile OTAError OTAWIFIAsyncBar_errorCode =
    OTAError::NONE;


// =====================================================
// TIMEOUT
// =====================================================

// 5 seconds without receiving data
static const unsigned long OTA_TIMEOUT = 5000;

volatile unsigned long OTAWIFIAsyncBar_lastDataTime = 0;


// =====================================================
// CONSTRUCTOR
// =====================================================

OTAWIFIAsyncBar::OTAWIFIAsyncBar() {

    _server = nullptr;
}


// =====================================================
// BEGIN
// =====================================================

void OTAWIFIAsyncBar::begin(
    AsyncWebServer *server
) {

    _server = server;


    // =============================================
    // RESET STATE
    // =============================================

    OTAWIFIAsyncBar_progress = 0;

    OTAWIFIAsyncBar_active = false;

    OTAWIFIAsyncBar_error = false;

    OTAWIFIAsyncBar_errorCode =
        OTAError::NONE;

    OTAWIFIAsyncBar_lastDataTime = 0;


    // =============================================
    // OTA PAGE
    // =============================================

    _server->on(
        "/update",
        HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send(
                200,
                "text/html",
                OTAWIFIAsyncBarPage
            );
        }
    );


    // =============================================
    // OTA UPLOAD
    // =============================================

    _server->on(
        "/update",
        HTTP_POST,

        // =========================================
        // POST COMPLETE
        // =========================================

        [](AsyncWebServerRequest *request) {

            // -------------------------------------
            // Error already detected
            // -------------------------------------

            if (OTAWIFIAsyncBar_error) {

                request->send(
                    200,
                    "text/plain",
                    "FAIL"
                );

                return;
            }


            // -------------------------------------
            // Check Update
            // -------------------------------------

            bool error = Update.hasError();


            if (error) {

                OTAWIFIAsyncBar_error = true;

                OTAWIFIAsyncBar_active = false;


                if (
                    OTAWIFIAsyncBar_errorCode ==
                    OTAError::NONE
                ) {

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::UNKNOWN;
                }


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

            }

            else {

                OTAWIFIAsyncBar_error = false;

                OTAWIFIAsyncBar_active = false;

                OTAWIFIAsyncBar_errorCode =
                    OTAError::NONE;
            }


            // =====================================
            // HTTP RESPONSE
            // =====================================

            request->send(
                200,
                "text/plain",
                error ? "FAIL" : "OK"
            );


            // =====================================
            // SUCCESS
            // =====================================

            if (!error) {

                Serial.println();
                Serial.println(
                    "========== OTA SUCCESS =========="
                );

                Serial.println(
                    "Restart ESP32..."
                );

                delay(500);

                ESP.restart();
            }
        },


        // =========================================
        // UPLOAD CALLBACK
        // =========================================

        [](AsyncWebServerRequest *request,
           String filename,
           size_t index,
           uint8_t *data,
           size_t len,
           bool final) {


            // =====================================
            // START OF FILE
            // =====================================

            if (index == 0) {

                OTAWIFIAsyncBar_progress = 0;

                OTAWIFIAsyncBar_active = true;

                OTAWIFIAsyncBar_error = false;

                OTAWIFIAsyncBar_errorCode =
                    OTAError::NONE;


                OTAWIFIAsyncBar_lastDataTime =
                    millis();


                Serial.println();
                Serial.println(
                    "========== OTA START =========="
                );

                Serial.print(
                    "File: "
                );

                Serial.println(filename);

                Serial.print(
                    "HTTP size: "
                );

                Serial.println(
                    request->contentLength()
                );


                // =================================
                // CHECK FILE EXTENSION
                // =================================

                String lowerFilename =
                    filename;

                lowerFilename.toLowerCase();


                if (
                    !lowerFilename.endsWith(
                        ".bin"
                    )
                ) {

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::INVALID_FILE;

                    OTAWIFIAsyncBar_active =
                        false;


                    Serial.println();
                    Serial.println(
                        "========== INVALID FILE =========="
                    );

                    Serial.print(
                        "Rejected file: "
                    );

                    Serial.println(filename);

                    Serial.println(
                        "Only .bin files are allowed."
                    );


                    return;
                }


                // =================================
                // START UPDATE
                // =================================

                if (
                    !Update.begin(
                        UPDATE_SIZE_UNKNOWN
                    )
                ) {

                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::BEGIN_FAILED;

                    OTAWIFIAsyncBar_active =
                        false;


                    Serial.println(
                        "OTA ERROR: Update.begin()"
                    );

                    return;
                }
            }


            // =====================================
            // DATA RECEIVED
            // =====================================

            if (len > 0) {

                OTAWIFIAsyncBar_lastDataTime =
                    millis();


                // =================================
                // WRITE DATA
                // =================================

                size_t written =
                    Update.write(
                        data,
                        len
                    );


                if (written != len) {

                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::WRITE_FAILED;

                    OTAWIFIAsyncBar_active =
                        false;


                    Serial.println(
                        "OTA ERROR: Update.write()"
                    );

                    return;
                }


                // =================================
                // PROGRESS
                // =================================

                size_t uploaded =
                    index + len;

                size_t total =
                    request->contentLength();


                if (total > 0) {

                    OTAWIFIAsyncBar_progress =
                        (uploaded * 100) / total;


                    if (
                        OTAWIFIAsyncBar_progress > 100
                    ) {

                        OTAWIFIAsyncBar_progress =
                            100;
                    }


                    Serial.print(
                        "OTA PROGRESS: "
                    );

                    Serial.print(
                        OTAWIFIAsyncBar_progress
                    );

                    Serial.print("% | ");

                    Serial.print(uploaded);

                    Serial.print("/");

                    Serial.println(total);
                }
            }


            // =====================================
            // END OF FILE
            // =====================================

            if (final) {

                // ---------------------------------
                // Error already detected
                // ---------------------------------

                if (
                    OTAWIFIAsyncBar_error
                ) {

                    return;
                }


                OTAWIFIAsyncBar_progress =
                    100;


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


                // =================================
                // FINALIZE UPDATE
                // =================================

                if (
                    Update.end(true)
                ) {

                    Serial.print(
                        "Update Success: "
                    );

                    Serial.println(
                        index + len
                    );

                }

                else {

                    Update.printError(
                        Serial
                    );

                    OTAWIFIAsyncBar_error =
                        true;

                    OTAWIFIAsyncBar_errorCode =
                        OTAError::END_FAILED;

                    OTAWIFIAsyncBar_active =
                        false;


                    Serial.println(
                        "OTA ERROR: Update.end()"
                    );
                }
            }
        }
    );
}


// =====================================================
// LOOP
// =====================================================

void OTAWIFIAsyncBar::loop() {

    if (
        !OTAWIFIAsyncBar_active
    ) {

        return;
    }


    // =============================================
    // TIMEOUT
    // =============================================

    if (
        millis() -
        OTAWIFIAsyncBar_lastDataTime
        >= OTA_TIMEOUT
    ) {

        OTAWIFIAsyncBar_error =
            true;

        OTAWIFIAsyncBar_errorCode =
            OTAError::TIMEOUT;

        OTAWIFIAsyncBar_active =
            false;


        Serial.println();
        Serial.println(
            "========== OTA TIMEOUT =========="
        );

        Serial.println(
            "No data received for 5 seconds."
        );


        // =========================================
        // ABORT
        // =========================================

        Update.abort();
    }
}


// =====================================================
// GET PROGRESS
// =====================================================

int OTAWIFIAsyncBar::getProgress() {

    return OTAWIFIAsyncBar_progress;
}


// =====================================================
// IS ACTIVE
// =====================================================

bool OTAWIFIAsyncBar::isActive() {

    return OTAWIFIAsyncBar_active;
}


// =====================================================
// HAS ERROR
// =====================================================

bool OTAWIFIAsyncBar::hasError() {

    return OTAWIFIAsyncBar_error;
}


// =====================================================
// GET ERROR
// =====================================================

OTAError OTAWIFIAsyncBar::getError() {

    return OTAWIFIAsyncBar_errorCode;
}


// =====================================================
// GET ERROR STRING
// =====================================================

String OTAWIFIAsyncBar::getErrorString() {

    switch (
        OTAWIFIAsyncBar_errorCode
    ) {

        case OTAError::NONE:
            return "No error";

        case OTAError::BEGIN_FAILED:
            return "Start failed";

        case OTAError::WRITE_FAILED:
            return "Write failed";

        case OTAError::END_FAILED:
            return "Finish failed";

        case OTAError::TIMEOUT:
            return "Timeout";

        case OTAError::INVALID_FILE:
            return "Invalid file";

        case OTAError::UNKNOWN:
            return "Unknown error";

        default:
            return "Unknown error";
    }
}


// =====================================================
// CLEAR ERROR
// =====================================================

void OTAWIFIAsyncBar::clearError() {

    OTAWIFIAsyncBar_error =
        false;

    OTAWIFIAsyncBar_errorCode =
        OTAError::NONE;

    OTAWIFIAsyncBar_progress =
        0;
}
