/*
 * ESP32Webserver.cpp
 *
 *  Created on: Okt 27, 2017
 *      Author: joseph
 *
 *  description:    
 */

#include "ESP32Webserver.h"
//TODO
  /*
   * creates all I/O Objects
   */
    SW_queue mySW_queue;

void initQueue_static(void* arg) {
    SHM *pSHM=(SHM*) arg;
    mySW_queue.SW_work(pSHM);
    vTaskDelete(NULL);
}

ESP32Webserver::ESP32Webserver(SHM* pSHM){
    //Serial.begin(115200);

    /*
     * 
     */
    printServerInformation();

    Serial.println("Configuring access point...");

    /*
     * 
     */
    mServer = new WiFiServer(80);     
    WiFi.softAP(mSsid, mPassword);

    IPAddress myIP = WiFi.softAPIP();
    mServerIP = String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]);
     
    printWifiServerInfo();

    Serial.println("Loading resource files ...");
    
    /*
     * 
     */
    SPIFFS.begin();
    loadResources();

    Serial.println("Creating SHM container ...");

    /*
     * 
     */
    mSHM=pSHM;
    Serial.println(mSHM->commonStart);

    /*
     * 
     */
    mServer->begin();
}

ESP32Webserver::~ESP32Webserver() {

}

void ESP32Webserver::printServerInformation() {
    Serial.println();
    Serial.println("-------------------------------------------------------------------------------------------------");
          
    Serial.print("Chip ID: ");
    Serial.println(ESP.getChipRevision());
         
    Serial.print("Free Heap Size: ");
    Serial.println(ESP.getFreeHeap());
         
    Serial.print("Chip Size: ");
    Serial.println(ESP.getFlashChipSize());
         
    Serial.print("Chip Speed: ");
    Serial.print((float)ESP.getFlashChipSpeed()/1000000);
    Serial.println("Mhz");
         
    Serial.print("Chip Mode: ");
    Serial.println(ESP.getFlashChipMode());

    Serial.println("-------------------------------------------------------------------------------------------------");
    Serial.println();
}

void ESP32Webserver::printWifiServerInfo() {
    byte mac[6];
  
    Serial.println();
    Serial.println("-------------------------------------------------------------------------------------------------");
  
    Serial.print("SSID: ");
    Serial.println(mSsid);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

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

    Serial.println("-------------------------------------------------------------------------------------------------");
    Serial.println();
}

void ESP32Webserver::loadResources(){   
    Serial.println();
    Serial.println("-------------------------------------------------------------------------------------------------");

    mSPIFFS.listDir(SPIFFS, "/", 0);
    Serial.println();

    Serial.println("-------------------------------------------------------------------------------------------------");
    Serial.println();
}

String ESP32Webserver::getWebPage() {
    //return mSPIFFS.GetWebPage();
}

String ESP32Webserver::getServerIPAddress() {
    return mServerIP;
}

String ESP32Webserver::getDataBody() {
    return mDataBody;
}
/*
void ESP32Webserver::createSHM(){   
    Serial.println();
    Serial.println("-------------------------------------------------------------------------------------------------");
    Serial.println();
    
    mSHM = new SHM();

    Serial.print("SHM created with status ");
    Serial.println(mSHM->commonStart);

    Serial.println();
    Serial.println("-------------------------------------------------------------------------------------------------");
    Serial.println();
}
*/
void ESP32Webserver::handleClient() {
    // listen for incoming clients
    WiFiClient client = mServer->available();
 
    if (client) {
        Serial.println();
        Serial.println("New client Connection");
        mLinebuf = "";
        mCharcount=0;
      
        // an http request ends with a blank line
        mCurrentLineIsBlank = true;
        mIsPostRequest = false;
        mCarriageReturnLineFeedCounter = 0;
        mDataBody = "";
        String postBuf = "";
        mURL = UNKNOWN;
        
        int contentLength = 0;

        Serial.println("-------------------------------------------------------------------------------------------------");
      
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.print(c);

                mLinebuf+=c;
                mCharcount++;

                if(c == '\n' && mCurrentLineIsBlank) {
                    mCarriageReturnLineFeedCounter++;
                }

                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if(mCarriageReturnLineFeedCounter == 1 && mIsPostRequest == false) {
                    Serial.println("No data body found");
                          
                    switch(mURL) {
                      case INDEX:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/html");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          client.println(mSPIFFS.getFile(SPIFFS, "/index.html"));
                      break;
                      case CSS:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/css");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          client.println(mSPIFFS.getFile(SPIFFS, "/stylesheet.css"));
                      break;
                      case BLOCKLY_COMPRESSED:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/blockly_compressed.js", client); 
                      break;
                      case CUSTOM_BLOCKS:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/custom_blocks.js", client); 
                      break;
                      case BLOCKS_COMPRESSED:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS,"/blocks_compressed.js", client); 
                      break;
                      case EN:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS,"/en.js", client);
                      break;
                      case LUA:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/lua.js", client); 
                      break;
                      case JAVASCRIPT:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/javascript.js", client); 
                      break;
                      case DART:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/dart.js", client); 
                      break;
                      case LOGIC:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();                      
                          mSPIFFS.printFileToClient(SPIFFS, "/logic.js", client); 
                      break;
                      case LOOPS:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/loops.js", client);
                      break;
                      case TEXT:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/text.js", client);
                      break;
                      case CUSTOM_BLOCKS_SYNTAX:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/custom_blocks_syntax.js", client);
                      break;
                      case MATH:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/math.js", client);
                      break;
                      case SPRITESPNG:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: image/png");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          client.println(mSPIFFS.getFile(SPIFFS, "/sprites.png"));
                      break;
                      case PRISMCSS:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/css");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/prism.css", client);
                      break;
                      case PRISMJS:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/javascript");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          mSPIFFS.printFileToClient(SPIFFS, "/prism.js", client);
                      break;
                      case STOP:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/html");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          client.println("Queue stoped");
                         
                          mSHM->commonStart = 0;
                          mSHM->commonPause = 0;
                          mSHM->commonStopp = 1;
                          
                          Serial.print("Program stopped with status ");
                          Serial.println(mSHM->commonStopp);
                      break;
                      case START:
                          if(mSHM->commonStart == 1 && mSHM->commonPause == 0 && mSHM->running == true) {
                              client.println("HTTP/1.1 400 ");
                              client.println("Content-Type: text/html");
                              client.println("Connection: close");  // the connection will be closed after completion of the response         
                              client.println();    
                              client.println("Queue already running");                                   
                          } else if(mSHM->commonStart == 0 && mSHM->running == false && mSHM->commonPause == 0) {
                              client.println("HTTP/1.1 200 OK");
                              client.println("Content-Type: text/html");
                              client.println("Connection: close");  // the connection will be closed after completion of the response
                              client.println();
                              client.println("Start New Queue"); 
                          
                              mSHM->commonStart = 1;
                              mSHM->commonPause = 0;
                              mSHM->commonStopp = 0;

                              
                              Serial.println(mSHM->webData.data);
                              

                              Serial.print("Start new queue with status ");
                              Serial.println(mSHM->commonStart);
                              
                              //TODO: Generate Queue!!!
                              // --> void function
                                xTaskCreatePinnedToCore(
                                                        initQueue_static,   /* Function to implement the task */
                                                        "initQueue_static", /* Name of the task */
                                                        4096,      /* Stack size in words */
                                                        (void*)mSHM,       /* Task input parameter */
                                                        1,          /* Priority of the task */
                                                        NULL,       /* Task handle. */
                                                        1);  /* Core where the task should run */
                              Serial.println("Task created!");
                              
                          } else if(mSHM->commonStart == 0 && mSHM->running == true && mSHM->commonPause == 1) {
                              client.println("HTTP/1.1 200 OK");
                              client.println("Content-Type: text/html");
                              client.println("Connection: close");  // the connection will be closed after completion of the response
                              client.println();
                              client.println("Continuing Queue");
                          
                              mSHM->commonStart = 1;
                              mSHM->commonPause = 0;

                              Serial.print("Queue continued with status ");
                              Serial.println(mSHM->commonStart);
                          } else {
                              client.println("HTTP/1.1 400 ");
                              client.println("Content-Type: text/html");
                              client.println("Connection: close");  // the connection will be closed after completion of the response         
                              client.println();    
                              client.println("Mega Critical Runtime Error");  
                          }
                      break;
                      case PAUSE:
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-Type: text/html");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();
                          client.println("Queue paused");

                          mSHM->commonStart = 0;
                          mSHM->commonPause = 1;
                          
                          Serial.print("Queue paused with status ");
                          Serial.println(mSHM->commonPause);
                      break;
                      case UNKNOWN:
                          client.println("HTTP/1.1 404 OK");
                          client.println("Connection: close");  // the connection will be closed after completion of the response
                          client.println();                          
                      break;
                    }      
                    break;
                } else if(mCarriageReturnLineFeedCounter == 1 && mIsPostRequest == true) {
                    if(mCharcount == contentLength) {
                        mDataBody = mLinebuf;                                                
                        
                        Serial.println();
                        Serial.println("{");
                        Serial.print("\t");
                        Serial.println(mDataBody);
                        Serial.println("}");

                        delay(10);

                        switch(mURL) {
                            case SEND:
                                client.println("HTTP/1.1 200 OK");
                                client.println("Content-Type: text/html");
                                client.println("Connection: close");  // the connection will be closed after completion of the response
                                client.println();
                                client.println("Queue string received");
                                  
                                mSHM->webData.data = mDataBody;
                                mSHM->webData.contentLength = contentLength;
                                
                                Serial.println("Queue string received");
                            break;
                            case UNKNOWN:
                                client.println("HTTP/1.1 404 OK");
                                client.println("Connection: close");  // the connection will be closed after completion of the response
                                client.println();   

                                Serial.println("Post request URL unknown");
                            break;
                            default:

                            break;
                        }
                        
                        break;
                    }
                }
          
                if (c == '\n') {
                      if(strstr(mLinebuf.c_str(),"POST /") > 0){
                          Serial.println("kind of request: Post request");
                          mIsPostRequest = true;

                          String tmp = (String)mLinebuf;
                          int blankIndex1 = tmp.indexOf(' ');
                          int blankIndex2 = tmp.indexOf(' ', blankIndex1 + 1);
                          Serial.print("-> URL: ");

                          String URL = tmp.substring(blankIndex1 + 1, blankIndex2);
                          Serial.println(URL);

                          if(URL == "/send") {
                              mURL = SEND;
                          } else {
                              mURL = UNKNOWN;
                          }
                          
                      } else if(strstr(mLinebuf.c_str(),"GET /") > 0){
                          Serial.println("-> kind of request: Get request");
                          
                          String tmp = (String)mLinebuf;
                          int blankIndex1 = tmp.indexOf(' ');
                          int blankIndex2 = tmp.indexOf(' ', blankIndex1 + 1);
                          Serial.print("-> URL: ");

                          String URL = tmp.substring(blankIndex1 + 1, blankIndex2);
                          Serial.println(URL);

                          if(URL == "/") {
                              mURL = INDEX;
                          } else if(URL == "/index.html") {
                              mURL = INDEX;
                          } else if(URL == "/stylesheet.css") {
                              mURL = CSS;
                          } else if(URL == "/blockly_compressed.js") {
                              mURL = BLOCKLY_COMPRESSED;
                          } else if(URL == "/custom_blocks.js") {
                              mURL = CUSTOM_BLOCKS;
                          } else if(URL == "/blocks_compressed.js") {
                              mURL = BLOCKS_COMPRESSED;
                          } else if(URL == "/en.js") {
                              mURL = EN;
                          } else if(URL == "/lua.js") {
                              mURL = LUA;
                          } else if(URL == "/javascript.js") {
                              mURL = JAVASCRIPT;
                          } else if(URL == "/dart.js") {
                              mURL = DART;
                          } else if(URL == "/logic.js") {
                              mURL = LOGIC;
                          } else if(URL == "/loops.js") {
                              mURL = LOOPS;
                          } else if(URL == "/text.js") {
                              mURL = TEXT;
                          } else if(URL == "/custom_blocks_syntax.js") {
                              mURL = CUSTOM_BLOCKS_SYNTAX;
                          } else if(URL == "/math.js") {
                              mURL = MATH;
                          } else if(URL == "/sprites.png") {
                              mURL = SPRITESPNG;
                          } else if(URL == "/prism.css") {
                              mURL = PRISMCSS;
                          } else if(URL == "/prism.js") {
                              mURL = PRISMJS;
                          } else if(URL == "/start") {
                              mURL = START;
                          } else if(URL == "/stop") {
                              mURL = STOP;
                          } else if(URL == "/pause") {
                              mURL = PAUSE;
                          } else {
                              Serial.println("Unknown URL");
                              mURL = UNKNOWN;                  
                          }
                      } else if(strstr(mLinebuf.c_str(),"Content-Length") > 0){
                          Serial.print("Setting size of message: ");       

                          String tmp = (String)mLinebuf;
                          int blankIndex1 = tmp.indexOf(' ');
                          contentLength = tmp.substring(blankIndex1 + 1).toInt();
                          
                          Serial.println(contentLength); 
                      }
    
                      // you're starting a new line
                      mCurrentLineIsBlank = true;
                      mLinebuf = "";
                      mCharcount=0;
                } else if(c != '\r') {
                    // you've gotten a character on the current line
                    mCurrentLineIsBlank = false;
                }   
            }
        }
      
        // give the web browser time to receive the data
        delay(10);
          
        Serial.println("-------------------------------------------------------------------------------------------------");
            
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
/*
void ESP32Webserver::initQueue((void*) pSHM) {

  mySW_queue.SW_work(mSHM);

}*/

