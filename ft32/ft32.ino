#include "ESP32Webserver.h"
#include "ft_ESP32_SHM.h"

ESP32Webserver * nServer;
SHM * ptrSHM;

void setup() {
    Serial.begin(115200);
    ptrSHM= new SHM;
    Serial.print("setup:");
    //Serial.println(ptrSHM->commonStart);
    nServer = new ESP32Webserver(ptrSHM);
}

void loop() {
    nServer->handleClient();
}


