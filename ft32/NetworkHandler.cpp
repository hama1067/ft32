/*
 * NetworkHandler.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: joseph
 */

#include "NetworkHandler.h"

NetworkHandler::NetworkHandler() {
	mSsid = "Espap";
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

void NetworkHandler::createUniqueAP(const char *pSsid, const char *pPassword) {
	Serial.print("[nwh] Creating AP ");
	Serial.print(pSsid);
	Serial.println("*");

	String test_ssid=pSsid;
	test_ssid=test_ssid+String((unsigned short)ESP.getEfuseMac());
	int numSsid = WiFi.scanNetworks();
	bool ssidExist;

	do {
		ssidExist=false;
		unsigned short rnd=1;

		for (int thisNet = 0; thisNet < numSsid; thisNet++) {
			if(WiFi.SSID(thisNet)==test_ssid) {
				ssidExist=true;
				test_ssid=pSsid+String((unsigned short)ESP.getEfuseMac()+rnd);
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

}

void NetworkHandler::joinExistingNetwork(const char *pSsid, const char *pPassword) {
	Serial.print("[nwh] Connecting to ");
	Serial.println(pSsid);

	mSsid = pSsid;
	mPassword = pPassword;

	WiFi.begin(mSsid, mPassword);

	Serial.print("[nwh] ");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	mIP = (String)WiFi.localIP();

	Serial.println();
	Serial.println("[nwh] WiFi connected");
	Serial.print("[nwh] SSID: ");
	Serial.println(mSsid);
	Serial.println("[nwh] IP: " + getIP());
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

