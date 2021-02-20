/*
 * AssetHandler.cpp
 *
 *  Created on: Okt 27, 2017
 *      Author: joseph
 *
 *  description:    
 */

#include "AssetHandler.h"

AssetHandler::AssetHandler() {
	mServer = new WiFiServer(80);

	Serial.println();
	Serial.println("[ash] creating AssetHandler object instance ...");
	#if ASSET_DEBUG_OUTPUT == 1
		Serial.println("[ash] WARNING: AssetHandler started with DEBUG_OUTPUT on!");
	#endif

	Serial.println("[ash] Loading resource files ...");
	Serial.println();
	//SPIFFS.begin(true);
	mSPIFFS.listDir(SPIFFS, "/", 0);

	mServer->begin();

	mCarriageReturnLineFeedCounter = 0;
	mCharcount = 0;
	mCurrentLineIsBlank = false;
	contentLength = 0;
	mIsPostRequest = false;
}

AssetHandler::~AssetHandler() {
	delete mServer;
}

String AssetHandler::getServerIPAddress() {
	return mServerIP;
}

String AssetHandler::getDataBody() {
	return mDataBody;
}

void AssetHandler::handleAssetRequests() {
	// listen for incoming clients
	WiFiClient *client = new WiFiClient(mServer->available());

	if (*client) {
		#if ASSET_DEBUG_OUTPUT == 1
        	Serial.println();
        	Serial.println("[ash] New incomming asset request");
		#endif

		mLinebuf = "";
		mCharcount = 0;

		// an http request ends with a blank line
		mCurrentLineIsBlank = true;
		mIsPostRequest = false;
		mCarriageReturnLineFeedCounter = 0;
		mDataBody = "";
		URL = "";
		contentLength = 0;

		while (client->connected()) {
			if (client->available()) {
				char c = client->read();
				//Serial.print(c);

				mLinebuf += c;
				mCharcount++;

				if (c == '\n' && mCurrentLineIsBlank) {
					mCarriageReturnLineFeedCounter++;
				}

				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (mCarriageReturnLineFeedCounter == 1
						&& mIsPostRequest == false) {
					String responseHeader = "";

					switch (generateHeader(URL, responseHeader)) {
					case 0:
						client->println(responseHeader);
						break;
					case 1:
						client->println(responseHeader);
						mSPIFFS.printFileToClient2(SPIFFS, URL.c_str(), client);
						break;
					case 2:
						client->println(responseHeader);
						mSPIFFS.printFileToClient2(SPIFFS,
								(URL + ".gz").c_str(), client);
						break;
					default:

						break;
					}

					break;
				} else if (mCarriageReturnLineFeedCounter == 1
						&& mIsPostRequest == true) {
					if (mCharcount == contentLength) {
						mDataBody = mLinebuf;
						break;
					}
				}

				if (c == '\n') {
					if (strstr(mLinebuf.c_str(), "POST /") > 0) {
						mIsPostRequest = true;
						URL = getURL(mLinebuf);

						#if ASSET_DEBUG_OUTPUT == 1
							Serial.println("[ash] kind of request: Post request");
							Serial.println("[ash] URL: " + URL);
						#endif
					} else if (strstr(mLinebuf.c_str(), "GET /") > 0) {
						URL = getURL(mLinebuf);

						#if ASSET_DEBUG_OUTPUT == 1
							Serial.println("[ash] kind of request: Get request");
							Serial.println("[ash] URL: " + URL);
						#endif

						if (URL == "/") {
							//redirect to index.html
							#if ASSET_DEBUG_OUTPUT == 1
								Serial.println("[ash] Redirecting to index.html");
							#endif

							URL = "/index.html";
						}

					} else if (strstr(mLinebuf.c_str(), "Content-Length") > 0) {
						String tmp = (String) mLinebuf;
						int blankIndex1 = tmp.indexOf(' ');
						contentLength = tmp.substring(blankIndex1 + 1).toInt();

						#if ASSET_DEBUG_OUTPUT == 1
							Serial.print("[ash] Setting size of message: ");
							Serial.println(contentLength);
						#endif
					}

					// you're starting a new line
					mCurrentLineIsBlank = true;
					mLinebuf = "";
					mCharcount = 0;
				} else if (c != '\r') {
					// you've gotten a character on the current line
					mCurrentLineIsBlank = false;
				}
			} else {

			}
		}

		// give the web browser time to receive the data
		delay(10);

		// close the connection:
		client->stop();

		#if ASSET_DEBUG_OUTPUT == 1
        	Serial.println("[ash] asset request successfully handled");
		#endif

		delete client;
	} else {
		delete client;
	}
}

int AssetHandler::generateHeader(String &pURL, String &pResponseHeader) {
	if (mSPIFFS.fileExists(SPIFFS, pURL.c_str())) {
		pResponseHeader = "HTTP/1.1 200 OK\n";
		pResponseHeader += getContentType(pURL) + "\n" + getContentLength(pURL)
				+ "\n" + "Connection: close" + "\n";
		return 1;
	} else if (mSPIFFS.fileExists(SPIFFS, (pURL + ".gz").c_str())) {
		pResponseHeader = "HTTP/1.1 200 OK\n";
		pResponseHeader += getContentType(pURL + ".gz") + "\n"
				+ getContentLength(pURL + ".gz") + "\n"
				+ "Content-Encoding: gzip" + "\n" + "Connection: close" + "\n";
		return 2;
	} else {
		pResponseHeader = "HTTP/1.1 404 Not Found\n";
		pResponseHeader += "Connection: close\n";
		return 0;
	}
}

String AssetHandler::getContentType(String &filePath) {
	String mContentTypeInformation = "";
	int dotIndex = filePath.indexOf('.');
	String type = filePath.substring(dotIndex + 1, filePath.length());

	if (type == "html") {
		mContentTypeInformation = "Content-Type: text/html";
	} else if (type == "js") {
		mContentTypeInformation = "Content-Type: application/javascript";
	} else if (type == "svg") {
		mContentTypeInformation = "Content-Type: image/svg+xml";
	} else if (type == "ttf") {
		mContentTypeInformation = "Content-Type: application/x-font-ttf";
	} else if (type == "css") {
		mContentTypeInformation = "Content-Type: text/css";
	} else if (type == "json") {
		mContentTypeInformation = "Content-Type: application/json";
	} else if (type == "png") {
		mContentTypeInformation = "Content-Type: image/png";
	} else if (type == "ico") {
		mContentTypeInformation = "Content-Type: image/x-icon";
	} else {
		mContentTypeInformation = "Content-Type: text/html";
	}

	return mContentTypeInformation;
}

String AssetHandler::getContentLength(String &filePath) {
	return "Content-Length: "
			+ String(mSPIFFS.getFileSize(SPIFFS, filePath.c_str()));
}

String AssetHandler::getURL(String &inputStream) {
	return inputStream.substring(inputStream.indexOf(' ') + 1,
			inputStream.indexOf(' ', inputStream.indexOf(' ') + 1));
}
