/*
 * NetworkHandler.h
 *
 *  Created on: Mar 1, 2018
 *      Author: joseph
 */

#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_

#include <WiFi.h>

class NetworkHandler {
public:
	NetworkHandler();
	~NetworkHandler();

	void createAP(const char *pSsid, const char *pPassword);

	void createUniqueAP(const char *pSsid, const char *pPassword);

	void joinExistingNetwork(const char *pSsid, const char *pPassword);

	String getIP();

	void printNetworkInformation();

private:
	String mIP;
	const char *mSsid;
	const char *mPassword;
};

#endif /* NETWORKHANDLER_H_ */
