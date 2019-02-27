#include "AssetHandler.h"
#include "WebsocketHandler.h"
#include "NetworkHandler.h"
#include "ft_ESP32_SHM.h"
#include "ft_ESP32_SW_Queue.h"
#include "ft_ESP32_IOobjects.h"
#include "CSpiffsStorage.h"
#include "CButton.h"
#include "DisableMotorBoard.h"
#include "OledHandler.h"

AssetHandler *nAssetHandler;          // Startet auch SPIFFS, also Objekt relativ am Anfang erstellen
WebsocketHandler *wsHandler;
NetworkHandler nHandler;
CheckMaxiExtension BoardType;

const char password_network[] = "63601430989011937932";
const char ssid_network[] = "HIT-FRITZBOX-7490";
const char password_ap[] = "12345678";
const char ssid_ap[] = "FT-CODY-";
String password = "";
String ssid = "";

SHM *ptrSHM;
SW_queue mySW_queue;
CButton hmiTaster;
Adafruit_SSD1306 display(4);

void initQueue_static(void* arg) {
    SHM *pSHM=(SHM*) arg;
    while(1)
    {
      if(true==pSHM->commonStart){mySW_queue.SW_work(pSHM);}
      else{delay(1);}
    }
    vTaskDelete(NULL);
}

void setup() {
    DisableMotorBoard();                        // Aussschalten Motortreiber
    Serial.begin(115200);
    initExtension();                            // Initialisieren des Extension Board
    bool maxiboard = BoardType.CheckMaxi();     // Erkennen welche Boards
    mySW_queue.setBoardType(maxiboard);         // Anpassen der Boardspezifischen Ports
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C
    InitOled(display);                          // Initialisieren des Displays
    ptrSHM = new SHM;                           // Anlegen Shared Memory
    // Abfrage welches Board (Maxi oder Mini) und festlegen des Tasters
    int TASTER_PIN;
    if (maxiboard == true){TASTER_PIN = 23;}            // Im Fall: Maxiboard erkannt
    else {TASTER_PIN = 35;}                             // Im Fall: Miniboard erkannt
    hmiTaster.initButton(TASTER_PIN, ptrSHM);           // Initialisieren des Tasters

    // Aufbau des Netzwerkes
    printVerbindungsaufbau(display);                      //Oled Ausgabe Verbinung wird aufgebaut
    if (digitalRead(TASTER_PIN) == false)
    {
      nHandler.joinExistingNetwork(ssid_network, password_network);
      Serial.println("Connected");
      password = String(password_network);
      ssid = String(ssid_network);
    }
    else
    {
      ssid = nHandler.createUniqueAP(ssid_ap, password_ap);
      password = String(password_ap);
    }
    printVerbunden(display);                             //Oled Ausgabe Verbinung aufgebaut
    ptrSHM->IPAdress = nHandler.getIP();
    nAssetHandler = new AssetHandler();
    wsHandler = new WebsocketHandler(ptrSHM);

    Serial.println("[main] Starting queue task");

    xTaskCreatePinnedToCore(
      initQueue_static,   	/* Function to implement the task */
      "initQueue_static", 	/* Name of the task */
      4096,      			/* Stack size in words */
      (void*)ptrSHM,       	/* Task input parameter */
      0,          			/* Priority of the task */
      NULL,       			/* Task handle. */
      1);  					/* Core where the task should run */

     printLoginData(display, ptrSHM->IPAdress, password, ssid);               //Ausgabe der IP Adresse SSID und Passwort des erreichbaren Netzwerks
}

void loop() {
    nAssetHandler->handleAssetRequests();
    wsHandler->handleWebSocketRequests();
    hmiTaster.handleButton();
}
