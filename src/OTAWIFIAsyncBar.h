#ifndef OTAWIFIAsyncBar_h
#define OTAWIFIAsyncBar_h

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>

class OTAWIFIAsyncBar {
public:
OTAWIFIAsyncBar();
void begin(AsyncWebServer *server);
int getProgress();
bool isActive();
bool hasError();

private:
AsyncWebServer *_server;
};

extern volatile int OTAWIFIAsyncBar_progress;
extern volatile bool OTAWIFIAsyncBar_active;
extern volatile bool OTAWIFIAsyncBar_error;

#endif
