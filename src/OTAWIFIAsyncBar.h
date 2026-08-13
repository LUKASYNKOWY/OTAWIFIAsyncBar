#ifndef OTAWIFIAsyncBar_h
#define OTAWIFIAsyncBar_h

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>


// =====================================================
// OTA ERROR CODES
// =====================================================

enum class OTAError : uint8_t {

    NONE = 0,

    BEGIN_FAILED,
    WRITE_FAILED,
    END_FAILED,
    TIMEOUT,
    INVALID_FILE,
    UNKNOWN
};


// =====================================================
// OTA CLASS
// =====================================================

class OTAWIFIAsyncBar {

public:

    OTAWIFIAsyncBar();

    void begin(AsyncWebServer *server);

    // Must be called regularly from loop
    void loop();

    int getProgress();

    bool isActive();

    bool hasError();

    OTAError getError();

    String getErrorString();

    void clearError();


private:

    AsyncWebServer *_server;
};


// =====================================================
// GLOBAL STATE
// =====================================================

extern volatile int OTAWIFIAsyncBar_progress;

extern volatile bool OTAWIFIAsyncBar_active;

extern volatile bool OTAWIFIAsyncBar_error;

#endif
