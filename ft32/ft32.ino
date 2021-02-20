/*
 * ft32.ino
 *
 *  Created on: Mar 04, 2019
 *      modified on: Jan 08, 2021
 *      Author: joseph
 *      version: v1.2.4 alpha
 *      current stable release: v1.2 beta (1.2)
 *  
 *  description:
 *  
 *  
 *  notifications:
 *  
 */

/* **************************************************************************** */
/*  
 *  set the current version number; should be equal to the current stable release
 *  number from github or above if its not a test version. 
 */
#define RELEASE_VERSION "1.2.4"

/*  
 *   specify the type of roboter we want to use
 *     -> 0: FT32 Fischertechnik roboter with A4990 motor driver
 *           (https://www.pololu.com/file/0J710/A4990-Datasheet.pdf)
 *     -> 1: eMalRob with DRV8833 dual motor driver
 *           (http://www.ti.com/lit/ds/symlink/drv8833.pdf)
 */
#define ROBOTER_VERSION 1
/* **************************************************************************** */

/* Neccessary asset, websocket, config and network connection handling includes */
#include "handler/AssetHandler.h"
#include "handler/WebsocketHandler.h"
#include "handler/NetworkHandler.h"
#include "handler/ConfigHandler.h"
/* **************************************************************************** */

/* Neccessary queue includes */
#include "shm/ft_ESP32_SHM.h"
#include "queue/ft_ESP32_SW_Queue.h"
#include "queue/ft_ESP32_IOobjects.h"

/* Additional Spiffs acces, HMI button and Oled includes */
#include "memory/CSpiffsStorage.h"
#include "misc/CButton.h"
#include "handler/OledHandler.h"
/* ***************************************************** */
//
///* Write protectet area, modifing is not allowed */
AssetHandler *nAssetHandler;
WebsocketHandler *wsHandler;
NetworkHandler nHandler;
ConfigHandler *nConfigHandler;

SHM *ptrSHM;
SW_queue *mySW_queue;

/* its highly recommended to change this key for every robot and make it NOT public! */
char *secureCipherKey = "abcdefghijklmnop";
/* ********************************************************************************* */

/* ********************************************* */

/* Additional hardware specific modifications */
CButton hmiTaster;
CheckMaxiExtension BoardType;
/* ********************************************/

void setup() {
	Serial.begin(115200);								// serial communication
	ptrSHM = new SHM;                          		 	// create shared memory
	SPIFFS.begin(true);                           		// initialize internal storage

	/* disable loop watchdog, causes reboot errors while loading spiffs content over wifi */

	disableLoopWDT();

	/* initialize system files and prepare to setup board parameters */

	nConfigHandler = new ConfigHandler(ptrSHM);
	nConfigHandler->setCipherKey(secureCipherKey);      // initialize cipher key
	nConfigHandler->checkup();    						// check system if all neccessary files exists

	/* initialize HMI button (connection button + start queue button) */

	hmiTaster.initButton(35, ptrSHM);

	/* check for wlanConfiguration file, try to connect, if fail, create an access point */

	cOledHandler::getInstance()->printOledMessage("Booting ...");

	bool connectToSavedWlan = false;
	String wlanConfiguration[2];

	if (digitalRead(35) == true) {
		// printing oled message: Check configuration file
		cOledHandler::getInstance()->printOledMessage(
				"Check configuration\nfile ...");

		if (nConfigHandler->checkNetworkConfigurationFile()) {
			// config file found and button not pressed to connect to saved, crypted wlanConfiguration.txt
			nConfigHandler->decipherNetworkConfigurationFile(wlanConfiguration);

			// printing oled message: connecting to existing network
			cOledHandler::getInstance()->printOledMessage("Connecting to wlan\n...\n\n=> " + wlanConfiguration[0]);

			// save decrypted ssid in wlanConfiguration[0],
			connectToSavedWlan = nHandler.joinExistingNetwork(wlanConfiguration[0].c_str(), wlanConfiguration[1].c_str());

			if (connectToSavedWlan) {
				nHandler.setSsid(wlanConfiguration[0].c_str());
				// -> "Connected to network"
			} else {
				cOledHandler::getInstance()->printOledMessage("Connection failed.\nCreating network ...");
				// -> "Access point created"
			}
		} else {
			cOledHandler::getInstance()->printOledMessage("No network config\nfile found!\nCreating network ...");
		}
	} else {
		cOledHandler::getInstance()->printOledMessage("Button pressed.\nCreating network ...");
		connectToSavedWlan = false; // -> "Access point created"
	}

	if (!connectToSavedWlan) {
		nHandler.createUniqueAP(); // -> Creating unique access point "FT-CODY-*"
	}

	/* creating AssetHandler (required for providing local codypp version from SPIFFS */
	/* creating WebsocketHandler to handle incomming websocket connections */
	/* creating queue object */

	ptrSHM->IPAdress = nHandler.getIP();
	nAssetHandler = new AssetHandler();
	wsHandler = new WebsocketHandler(ptrSHM);
	mySW_queue = new SW_queue();

	delay(10);
	Serial.println("[main] Starting queue task ...");

	/* creating initQueue_static thread to start/stop queue */
	mySW_queue->startQueueTask(ptrSHM);

	Serial.println("[main] => System is up and running.");
	Serial.println("[main] release version: r-" + String(RELEASE_VERSION));
	Serial.println("[main] selected roboter model: " + String(ROBOTER_VERSION == 0 ? "FT32" : "eMalRob"));

	/* print connection status */
	cOledHandler::getInstance()->printConnectionStatus(nHandler.getIP(), nHandler.getSsid(), nHandler.getMode());
}

void loop() {
	nAssetHandler->handleAssetRequests();
	wsHandler->handleWebSocketRequests();
	hmiTaster.handleButton();
	delay(1);
}
