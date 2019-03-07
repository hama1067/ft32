/*
 * NetworkHandler.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: joseph
 */

#include "NetworkHandler.h"

NetworkHandler::NetworkHandler() {
	mSsid = "FT-CODY-";
	mPassword = "12345678";
	mIP = "0.0.0.0";
}

NetworkHandler::~NetworkHandler() {

}

void NetworkHandler::createAP(const char *pSsid, const char *pPassword) {
	Serial.print("[nwh] Creating AP ");
	Serial.println(pSsid);

	mSsid = pSsid;
	mPassword = pPassword;

	WiFi.softAP(mSsid, mPassword);

	IPAddress myIP = WiFi.softAPIP();
	mIP = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);

	Serial.println();
	Serial.println("[nwh] AP created");
	Serial.print("[nwh] SSID: ");
	Serial.println(mSsid);
	Serial.println("[nwh] IP: " + getIP());
}

String NetworkHandler::createUniqueAP(const char *pSsid, const char *pPassword) {
	Serial.print("[nwh] Creating AP ");
	Serial.print(pSsid);
	Serial.println("*");

	String test_ssid=pSsid;
	char hex_serial_number[13];
  	sprintf(&hex_serial_number[0],"%X",(uint16_t)(ESP.getEfuseMac()>>32));
  	sprintf(&hex_serial_number[4],"%X",(uint32_t)ESP.getEfuseMac());
	test_ssid=test_ssid+hex_serial_number;
	int numSsid = WiFi.scanNetworks();
	bool ssidExist;

	do {
		ssidExist=false;
		unsigned short rnd=1;

		for (int thisNet = 0; thisNet < numSsid; thisNet++) {
			if(WiFi.SSID(thisNet)==test_ssid) {
				ssidExist=true;
				sprintf(hex_serial_number,"%x",(unsigned int)(ESP.getEfuseMac()+rnd));
				test_ssid=String(pSsid)+hex_serial_number;
				++rnd;
			}
		}
	} while(ssidExist);

	pSsid=test_ssid.c_str();

	mSsid = pSsid;
	mPassword = pPassword;

	WiFi.softAP(mSsid, mPassword);

	IPAddress myIP = WiFi.softAPIP();
	mIP = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);

	Serial.println();
	Serial.println("[nwh] AP created");
	Serial.print("[nwh] SSID: ");
	Serial.println(mSsid);
	Serial.println("[nwh] IP: " + getIP());

  return mSsid;

}

String NetworkHandler::createUniqueAP() {
  return createUniqueAP(mSsid, mPassword);
}

bool NetworkHandler::joinExistingNetwork(const char *pSsid, const char *pPassword) {
  bool returnCondition = false;
  int counter = 0;
  int connectionTries = 5;
  int connectionTryCounter = 0;

	WiFi.begin(pSsid, pPassword);

	while (WiFi.status() != WL_CONNECTED && connectionTryCounter < connectionTries) {
    if(counter == 0) {  
      Serial.print("[nwh] Connecting to ");
      Serial.println(pSsid);
      Serial.print("[nwh] ");
    }
    
		delay(500);
		
		Serial.print(".");
    counter++;
    
    if (counter == 10) {
      counter = 0;
      connectionTryCounter++;
      
      if(connectionTryCounter < connectionTries) {
        Serial.println();
        Serial.print("[nwh] Failed: Can not connect to given WiFi. Restart WiFi for another try [");
        Serial.print(connectionTryCounter);
        Serial.println("].");
        
        WiFi.reconnect();
        delay(1000);
      } else {
        Serial.println();
        Serial.println("[nwh] Failed: Can not connect to given WiFi. Aborted!");
      }
    }
	}

  if( WiFi.status() == WL_CONNECTED ) {
    
    mSsid = pSsid;
    mPassword = pPassword;

    IPAddress myIP = WiFi.localIP();
    mIP = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
    
    Serial.println();
    Serial.println("[nwh] WiFi connected");
    Serial.print("[nwh] SSID: ");
    Serial.println(mSsid);
    Serial.println("[nwh] IP: " + getIP());

    returnCondition = true;
  } else {
    WiFi.disconnect();
    returnCondition = false;
  }

  return returnCondition;
}

String NetworkHandler::getIP() {
	return mIP;
}

void NetworkHandler::printNetworkInformation() {
	byte mac[6];

	Serial.println();

	Serial.print("SSID: ");
	Serial.println(mSsid);
	Serial.print("IP Address: ");
	Serial.println(mIP);
	WiFi.macAddress(mac);
	Serial.print("Device MAC Address: ");
	for(int i = 5; i >= 1; i--) {
		Serial.print(mac[i], HEX);
		Serial.print(":");
	}
	Serial.println(mac[0]);
	Serial.print("Signal strength (RSSI): ");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm");

	Serial.println();
}
