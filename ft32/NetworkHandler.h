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

	String createUniqueAP(const char *pSsid, const char *pPassword);

	String createUniqueAP();

	bool joinExistingNetwork(const char *pSsid, const char *pPassword);

	String getIP();
  void setIP(String pIP);
	
	String getSsid();
  void setSsid(const char *pSsid);
	
	int getMode();
  void setMode(String pMode);

	void printNetworkInformation();

private:
	String mIP;
  String mSSID;
  int connectionMode;  // 0=connected to network, 1=accesspoint created
 
	const char *mSsid;
	const char *mPassword;
};

#endif /* NETWORKHANDLER_H_ */
