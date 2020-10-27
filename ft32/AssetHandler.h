/*
 * AssetHandler.h
 *
 *  Created on: Okt 27, 2017
 *      Author: joseph
 *
 *  description:    
 */

#ifndef ASSETHANDLER_H_
#define ASSETHANDLER_H_

#include <WiFi.h>
#include <Esp.h>
#include "SPIFFSTest.h"

class AssetHandler {
public:
    /*
     *  our default constructor
     */
	 AssetHandler();

    /*
     *   default destructor
     */
	 ~AssetHandler();

    /*
     *   display Chip ID, Heap Size, Chip Size, Chip Speed
     *   and Chip Mode of the ESP32 microcontroller
     */
    void printServerInformation();
    
    /*
     *   function, which handles the incomming requests (get/post requests)
     */
    void handleAssetRequests();

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
     *   Generates the html response header
     *   0 -> requested file not found
     *   1 -> requested file found
     *   2 -> requested file found with extension *.gz
     */
    int generateHeader(String &pURL, String &pResponseHeader); 

    /*
     *   Get Content-Type of a specific file
     */
    String getContentType(String &filePath);

    /*
     *   Get Content-Length of the requested file
     */
    String getContentLength(String &filePath);

    /*
     *   Get requested URL
     */
    String getURL(String &inputStream);

private:
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
     *   number of characters when a post request with data available is incomming
     */
     int contentLength;

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

    /*
     *   SPIFFS Container / Storage
     */
    CSPIFFS mSPIFFS;

    /*
     *   Requested URL
     */
    String URL = "";
   
};

#endif /* ASSETHANDLER_H_ */
