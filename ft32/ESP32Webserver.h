/*
 * ESP32Webserver.h
 *
 *  Created on: Okt 27, 2017
 *      Author: joseph
 *
 *  description:    
 */

#ifndef ESP32WEBSERVER_H_
#define ESP32WEBSERVER_H_

#include <WiFi.h>
#include <Esp.h>
#include "SPIFFSTest.h"
#include "httpURL.h"

#include "ft_ESP32_SW_Queue.h"

class ESP32Webserver {
public:
    /*
     *  our default constructor
     */
	  ESP32Webserver(SHM* pSHM);

    /*
     *   default destructor
     */
	  ~ESP32Webserver();

    /*
     *   display Chip ID, Heap Size, Chip Size, Chip Speed
     *   and Chip Mode of the ESP32 microcontroller
     */
    void printServerInformation();

    /*
     *   display ssid, IP address, MAC address and signal strength
     */
    void printWifiServerInfo();

    /*
     *   displays the content of the root file system 
     *   load neccessary resources (index.html, javascript files ...)
     */
    void loadResources();
    
    /*
     *   function, which handles the incomming requests (get/post requests)
     */
    void handleClient();

    /*
     *   returns the index.html file as string
     */
    String getWebPage(); /* TODO: -> reading Spiffs*/

    /*
     *   get the ip address of the server
     *   default: 192.168.4.1
     */
    String getServerIPAddress();

    /*
     *   if a post request is incomming, the content of the request will be stored in this string
     *   After that it will be copied in th webData SHM.
     */
    String getDataBody();

    /*
     *   create Queue
     */
    //void initQueue((void*) pSHM);

 
    /* TODO remove
     *   creates the SHM container
    void createSHM();
    */
private:
    /* 
     *   ssid and password is needed to create an Wifi 
     *   access point
     *   TODO: -> auto generated ssid?
     */
    const char *mSsid = "ESP32ap";
    const char *mPassword = "12345678"; 

    /*
     *   neccessary to read http get/post requests char by char
     */
    int mCharcount;
    String mLinebuf;

    /*
     *   if you've gotten to the end of the line (received a newline
     *   character) and the line is blank, the http request has ended
     *   if you know that it is a post request, you have to wait until
     *   the data body of the request is readout
     */
    bool mCurrentLineIsBlank, mIsPostRequest;
    
    /*
     *   count when a new blank line appears
     */
    int mCarriageReturnLineFeedCounter;

    /*
     *   WiFI Sever Object creates an server listner on port 80
     */   
    WiFiServer *mServer;

    /*
     *   store server ip address
     */
    String mServerIP;

    /*
     *   post request data body storage
     */
    String mDataBody;
    char *mBuffData;

    /*
     *   allow console output for debug purpose
     */
    bool mDebugMode;

    /*
     *   SPIFFS Container / Storage
     */
    CSPIFFS mSPIFFS;

    /*
     *   URL Enum file
     */
    httpURL mURL;

    /*
     *   SHM Container to exchange information between several threads, in this cas to exchange data
     *   between the Queue and the WebServer application
     */
    SHM *mSHM;
};

#endif /* ESP32WEBSERVER_H_ */
