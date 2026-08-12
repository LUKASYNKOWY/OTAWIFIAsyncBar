#include "OTAWIFIAsyncBar.h"
#include "OTAWIFIAsyncBarPage.h"

volatile int OTAWIFIAsyncBar_progress=0;
volatile bool OTAWIFIAsyncBar_active=false;
volatile bool OTAWIFIAsyncBar_error=false;

OTAWIFIAsyncBar::OTAWIFIAsyncBar(){
_server=nullptr;
}

void OTAWIFIAsyncBar::begin(AsyncWebServer *server){
_server=server;

_server->on("/update",HTTP_GET,[](AsyncWebServerRequest *request){
request->send(200,"text/html",OTAWIFIAsyncBarPage);
});

_server->on("/update",HTTP_POST,[](AsyncWebServerRequest *request){
bool error=Update.hasError();
OTAWIFIAsyncBar_error=error;
OTAWIFIAsyncBar_active=false;

request->send(200,"text/plain",error?"FAIL":"OK");

if(!error){
delay(500);
ESP.restart();
}
},[](AsyncWebServerRequest *request,String filename,size_t index,uint8_t *data,size_t len,bool final){

if(index==0){
OTAWIFIAsyncBar_progress=0;
OTAWIFIAsyncBar_active=true;
OTAWIFIAsyncBar_error=false;

Serial.println();
Serial.println("========== OTA START ==========");
Serial.print("Plik: ");
Serial.println(filename);
Serial.print("Rozmiar HTTP: ");
Serial.println(request->contentLength());

if(!Update.begin(UPDATE_SIZE_UNKNOWN)){
Update.printError(Serial);
OTAWIFIAsyncBar_active=false;
OTAWIFIAsyncBar_error=true;
return;
}
}

if(len>0){
if(Update.write(data,len)!=len){
Update.printError(Serial);
OTAWIFIAsyncBar_error=true;
}

size_t uploaded=index+len;
size_t total=request->contentLength();

if(total>0){
OTAWIFIAsyncBar_progress=(uploaded*100)/total;

if(OTAWIFIAsyncBar_progress>100){
OTAWIFIAsyncBar_progress=100;
}

Serial.print("OTA POSTEP: ");
Serial.print(OTAWIFIAsyncBar_progress);
Serial.print("% | ");
Serial.print(uploaded);
Serial.print("/");
Serial.println(total);
}
}

if(final){
OTAWIFIAsyncBar_progress=100;

Serial.println();
Serial.println("========== OTA END ==========");
Serial.print("Przeslano: ");
Serial.println(index+len);

if(Update.end(true)){
Serial.print("Update Success: ");
Serial.println(index+len);
}else{
Update.printError(Serial);
OTAWIFIAsyncBar_error=true;
OTAWIFIAsyncBar_active=false;
}
}
});
}

int OTAWIFIAsyncBar::getProgress(){
return OTAWIFIAsyncBar_progress;
}

bool OTAWIFIAsyncBar::isActive(){
return OTAWIFIAsyncBar_active;
}

bool OTAWIFIAsyncBar::hasError(){
return OTAWIFIAsyncBar_error;
}
