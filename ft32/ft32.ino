/*
 * ft32.ino
 *
 *  Created on: Mar 04, 2019
 *      Author: joseph
 *      version: 1.5 beta
 *  
 *  description:
 *  
 *  
 *  notifications:
 *  
 */

/* Neccessary asset, websocket, config and network connection handling includes */
#include "AssetHandler.h"
#include "WebsocketHandler.h"
#include "NetworkHandler.h"
#include "ConfigHandler.h"

/* Neccessary queue includes */
#include "ft_ESP32_SHM.h"
#include "ft_ESP32_SW_Queue.h"
#include "ft_ESP32_IOobjects.h"

/* Additional Spiffs acces, HMI button and Oled includes */
#include "CSpiffsStorage.h"
#include "CButton.h"
#include "DisableMotorBoard.h"
#include "OledHandler.h"

/* Write protectet area, modifing is not allowed */
AssetHandler *nAssetHandler;
WebsocketHandler *wsHandler;
NetworkHandler nHandler;
ConfigHandler *nConfigHandler;

SHM *ptrSHM;
SW_queue mySW_queue;

/* its highly recommended to change this key for every robot and make it NOT public! */
char * secureCipherKey = "we8ogr78owt346troga";
/* ********************************************************************************* */

/* ***********************************************/

/* Additional hardware specific modifications */
CButton hmiTaster;
Adafruit_SSD1306 display(4);
CheckMaxiExtension BoardType;
/* ********************************************/

void initQueue_static(void* arg) {
    SHM *pSHM=(SHM*) arg;

    while(1) {
      if(true==pSHM->commonStart){
    	  mySW_queue.SW_work(pSHM);
      } else{
    	  delay(1);
      }
    }

    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);											                    // serial communication
    ptrSHM = new SHM;                          		 			    	    // create shared memory
    SPIFFS.begin(true);                                           // initialize internal storage
    
    /* initialize system files and prepare to setup board parameters */

    nConfigHandler = new ConfigHandler(ptrSHM);
    nConfigHandler->setCipherKey(secureCipherKey);                // initialize cipher key
    nConfigHandler->checkup();                                    // check system if all neccessary files exists
    
    /* initialize board specific parameters like oled, buttons, motor drivers */

    DisableMotorBoard();                        					        // turn off motor driver
    initExtension();                            					        // initialize extension board

    bool maxiboard = BoardType.CheckMaxi();     					        // detecting board type (small or big one -> HWG, HWK)
    mySW_queue.setBoardType(maxiboard);         					        // set board specific port properties 
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  					        // initialize with the I2C addr 0x3C

    InitOled(display);                          					        // initialize oled display

    /* detecting board type, depending one which board we have to set the right button pin number */

    int TASTER_PIN;  
    (maxiboard == true) ? TASTER_PIN = 23 : TASTER_PIN = 35; 	    // case true:  maxi board (HWG) detected, case false: mini board (HWK) detected   
    hmiTaster.initButton(TASTER_PIN, ptrSHM);           			    // initialize hmi button (connection button + start queue button)

    /* check for wlanConfiguration file, try to connect, if fail, create an access point */

    printOledMessage(display, "Booting ...");

    String ssid = "";
    bool connectToSavedWlan = false;
    String wlanConfiguration[2];
    int connectionMode = 0;

    if ( digitalRead(TASTER_PIN) == false && nConfigHandler->checkNetworkConfigurationFile() ) {
      // printing oled message: connecting to existing network
      printOledMessage(display, "Connecting to wlan\n...");
      
      // config file found and button pressed to connect to saved, crypted wlanConfiguration.txt      
      nConfigHandler->decipherNetworkConfigurationFile(wlanConfiguration);

      // save decrypted ssid in wlanConfiguration[0], 
      connectToSavedWlan = nHandler.joinExistingNetwork(wlanConfiguration[0].c_str(), wlanConfiguration[1].c_str());
      
      if( connectToSavedWlan ) {
        ssid = wlanConfiguration[0];
        connectionMode = 0;             // -> "Connected to network"
      } else {
        printOledMessage(display, "Connection failed.\nCreating network ...");
        connectionMode = 1;             // -> "Access point created"
      }
    } else {
      printOledMessage(display, "No network config\nfile found or button pressed.\nCreating network ...");
      connectionMode = 1;               // -> "Access point created"
      connectToSavedWlan = false;     
    }

    if( !connectToSavedWlan ) {
      ssid = nHandler.createUniqueAP(); // -> Creating unique access point "FT-CODY-*"
    }
    
    printConnectionStatus(display, nHandler.getIP(), ssid, connectionMode);

    /* creating AssetHandler (required for providing local codypp version from SPIFFS */
    /* creating WebsocketHandler to handle incomming websocket connections */
    /* creating initQueue_static thread to start/stop queue */

    ptrSHM->IPAdress = nHandler.getIP();
    nAssetHandler = new AssetHandler();
    wsHandler = new WebsocketHandler(ptrSHM);

    delay(10);
    Serial.println("[main] Starting queue task.");

    xTaskCreatePinnedToCore(
      initQueue_static,   											// Function to implement the task
      "initQueue_static", 											// Name of the task
      4096,      													      // Stack size in words
      (void*)ptrSHM,       											// Task input parameter
      0,          													    // Priority of the task
      NULL,       													    // Task handle.
      1);  															        // Core where the task should run
    
    Serial.println("[main] System is up and running.");
}

void loop() {
    nAssetHandler->handleAssetRequests();
    wsHandler->handleWebSocketRequests();
    hmiTaster.handleButton();
}
