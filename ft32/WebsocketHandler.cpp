#include "WebsocketHandler.h"

extern WebsocketHandler *wsHandler;
extern ConfigHandler *nConfigHandler;
extern SHM * ptrSHM;

void webSocketTask(void* params) {
  WiFiClient* nClient = (WiFiClient*) params;
  WebSocketServer* nWebSocketServer = new WebSocketServer();
  
  String *data = new String();
  String *payload = new String();
  String *identifier = new String();

  if(wsHandler->addWebSocketClient(nClient, nWebSocketServer)) {
    if(nClient->connected() && nWebSocketServer->handshake(*nClient)) {
      int websocketClientID = wsHandler->getClientID(nClient);
      
      Serial.print("[ws] Handshake with client ");
      Serial.print(websocketClientID);
      Serial.println(" successfully done.");
      
      Serial.println("[ws] Establishing connection...");
      
      // Update state
      if( ptrSHM->commonStart == 1 && ptrSHM->commonPause == 0 ) {
    	  wsHandler->sendWebSocketMessage("running");
      } else if( ptrSHM->commonPause == 1 ) {
    	  wsHandler->sendWebSocketMessage("paused");
      } else if (ptrSHM->mSpeicher.getFileSize("/spiffs-cody-storage.txt") > 0) {
    	  wsHandler->sendWebSocketMessage("ready");
      }

      // Timer flag states
      wsHandler->setWebsocketFlag(websocketClientID, true);
      wsHandler->setWebsocketPingReceived(websocketClientID, true);

      while (nClient->connected()) {
        *data = nWebSocketServer->getData();
        
        delay(10); // Delay needed for receiving the data correctly
        
        if (data->length() > 0) {
          int delimiterIndex = data->indexOf('/');
          *payload = data->substring(delimiterIndex + 1, data->length());
          *identifier = data->substring(0, delimiterIndex);
        
          if(*identifier == "send") {
            // command string received, saving to spiffs
            Serial.println("[ws] send");

            ptrSHM->webData.data = *payload;
            ptrSHM->webData.contentLength = (*payload).length();

            // save to spiffs
            int counter = 0;
            bool loop_var = true;
            while(loop_var) {
              if ((ptrSHM->mSpeicher.save(*payload) == true)||(counter > 10)) {
                  loop_var = false;
              } else {
                  counter++; 
              }
            }
            // end save to spiffs

            Serial.println("[ws] Payload: " + ptrSHM->webData.data);
            wsHandler->sendWebSocketMessage("ready");
          } else if(*identifier == "start") {
            // start queue
            Serial.println("[ws] start");
            ptrSHM->commonStart = 1;
            ptrSHM->commonPause = 0;

            wsHandler->sendWebSocketMessage("running");    
          } else if(*identifier == "pause") {
            // pause queue
            Serial.println("[ws] pause");

            if(ptrSHM->running == true && ptrSHM->commonPause == 1) {
              //nWebSocketServer->sendData("Error: Already paused");
              wsHandler->sendWebSocketMessage("Error: Already paused");
            } else {
              //nWebSocketServer->sendData("paused");
              wsHandler->sendWebSocketMessage("paused");
              ptrSHM->commonPause = 1;
            }

            
          } else if(*identifier == "stop") {
            // stop queue
            Serial.println("[ws] stop");

            ptrSHM->commonStart = 0;
            ptrSHM->commonPause = 0;
            
            wsHandler->sendWebSocketMessage("stopped");
          } else if(*identifier == "config") {
            // wlan config received, save to spiffs and cipher the config content
            Serial.println("[ws] config received");

            if( nConfigHandler->cipherNetworkConfigurationFile(payload) ) {
              Serial.println("[ws] Saved ciphered configuration. FT32 can now reboot.");
              wsHandler->sendWebSocketMessageToClient(nClient, "received");

              if(ptrSHM->running != true) {
                Serial.println("[ws] FT32 rebooting ...");
                delay(1000);
                ESP.restart();
              } else {
                Serial.println("[ws] FT32 running state is true. Reboot aborted!");
              }
            } else {
              Serial.println("[ws] Error: Can not save ciphered configuration.");
              wsHandler->sendWebSocketMessageToClient(nClient, "error");
            }
          } else if(*identifier == "ping") {
            /* Serial.print("[ws] Received msg from client ");
            Serial.print(wsHandler->getClientID(nClient));
            Serial.println(": ping"); */

            // ping event -> flag to true
            wsHandler->setWebsocketFlag(websocketClientID, true);
            wsHandler->sendWebSocketMessageToClient(nClient, "pong");   
          }
        }

        if( wsHandler->handleWebsocketTimeout(websocketClientID) ) {
          Serial.println("[ws] Error: websocket timed out!");
          Serial.println("[ws] Close websocket connection ...");
          break;
        }
      }

      Serial.println("[ws] after while loop!");
    }
  } else {
    Serial.println("[ws] Maximum client connections reached.");
    delete nClient;
  }
  
  wsHandler->removeWebSocketClient(nClient);

  delete data;
  delete payload;
  delete identifier;

  Serial.print("[ws] free heap: ");
  Serial.println(ESP.getFreeHeap());
  
  vTaskDelete(NULL);
}

void eventListener(void* params) {
  bool last = false;
  bool current = false;

  Serial.println("[ws] Starting event listener ...");
  
  while(1) {
    current = ptrSHM->running;

    if(last != current) {
      Serial.println("changed");
    }
    
    if(last != current && current == 0) {
      wsHandler->sendWebSocketMessage("stopped");
      last = false;
      current = false;
    }
    
    if(last != current && current == 1) {
      wsHandler->sendWebSocketMessage("running");
      last = true;
      current = true;
    }
    //delay(10);
    last = current;
  }
  vTaskDelete(NULL);
}

WebsocketHandler::WebsocketHandler(SHM *pSHM) {
  Serial.println();
  Serial.print("[ws] Joining SHM container with status ");
  mSHM=pSHM;
  Serial.println(mSHM->commonStart);

  Timer::getInstance()->setTimeout(5);
  Timer::getInstance()->setCallbackFunction(websocketTimeoutEvent);
  
  if(!Timer::getInstance()->start()) {
    Serial.println("[timer] Can not start timer!");
  }
  
  addClient.unlock();
  removeClient.unlock();
  sendData.unlock();
  clientID.unlock();
  accessFlag.unlock();
  
  clientCount = 0;
  webSocketServer = new WiFiServer(90);

  webSocketConnections = new WebSocketConnection*[MAXCLIENTS];

  for(int i = 0; i < MAXCLIENTS; i++) {
    setWebsocketFlag(i, true);
    setWebsocketPingReceived(i, true);
  }

  for(int i = 0; i < MAXCLIENTS; i++) {
    webSocketConnections[i] = NULL;
    //connectedClients[i] = NULL;
  }

  xTaskCreatePinnedToCore(
    eventListener,    /* Function to implement the task */
    "",               /* Name of the task */
    4096,             /* Stack size in words */
    NULL,             /* Task input parameter */
    0,                /* Priority of the task */
    NULL,             /* Task handle. */
    1);               /* Core where the task should run */

  webSocketServer->begin();
}

WebsocketHandler::~WebsocketHandler() {
  delete webSocketConnections;
  delete webSocketServer;
}

bool WebsocketHandler::addWebSocketClient(WiFiClient * pClient, WebSocketServer *nWebSocketServer) {
  addClient.lock();
  
  if(clientCount >= MAXCLIENTS) {
    addClient.unlock();
  } else {
    for(int i = 0; i < MAXCLIENTS; i++) {
      if(webSocketConnections[i] == NULL) {
        webSocketConnections[i] = new WebSocketConnection(pClient, nWebSocketServer);
        
        Serial.print("[ws] Adding client ");
        Serial.println(i);
        clientCount++;
        addClient.unlock();
        return true;
      }
    }
  }

  addClient.unlock();

  return false;
}

bool WebsocketHandler::removeWebSocketClient(WiFiClient * pClient) {
  removeClient.lock();
  
  for(int i = 0; i < MAXCLIENTS; i++) {
    if(webSocketConnections[i] != NULL) {
        if(webSocketConnections[i]->pClient == pClient) {
        delete webSocketConnections[i]->pClient;
        delete webSocketConnections[i]->pWebSocketServer;
        delete webSocketConnections[i];
        webSocketConnections[i] = NULL;
  
        Serial.print("[ws] Removing client ");
        Serial.println(i);
        clientCount--;
        removeClient.unlock();
        return true;
      }
    }
  }
  
  removeClient.unlock();
  return false;
}

void WebsocketHandler::sendWebSocketMessage(String message) {
  sendData.lock();

  for(int i = 0; i < MAXCLIENTS; i++) {
    if(webSocketConnections[i] != NULL) {
      Serial.print("[ws] Sending msg to client ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(message);
      
      webSocketConnections[i]->pWebSocketServer->sendData(message);
    }
  }

  sendData.unlock();
}

void WebsocketHandler::sendWebSocketMessageToClient(WiFiClient * pClient, String msg) {
  sendData.lock();
  int clientID = getClientID(pClient);
  
  Serial.print("[ws] Sending msg to client ");
  Serial.print(clientID);

  if(clientID != -1 && webSocketConnections[clientID] != NULL) {
    Serial.print(" : ");
    Serial.println(msg);
    
    webSocketConnections[clientID]->pWebSocketServer->sendData(msg);
  } else {
    Serial.println(" failed. Client is removed or not available anymore.");
  }

  sendData.unlock();
}

int WebsocketHandler::getClientID(WiFiClient * pClient) {
  clientID.lock();
  
  for(int i = 0; i < MAXCLIENTS; i++) {
    if (webSocketConnections[i] != NULL)
    {
      if(webSocketConnections[i]->pClient == pClient) {
        clientID.unlock();
        return i;
      }
    }
  }
  
  clientID.unlock();
  return -1;
}

void WebsocketHandler::handleWebSocketRequests() {
  WiFiClient* client = new WiFiClient(webSocketServer->available());
  
  if (*client) {
    //Serial.println("New client found");
    xTaskCreatePinnedToCore(
      webSocketTask,    /* Function to implement the task */
      "",               /* Name of the task */
      10000,            /* Stack size in words */
      (void*)client,    /* Task input parameter */
      0,                /* Priority of the task */
      NULL,             /* Task handle. */
      1);               /* Core where the task should run */
  } else {
    delete client;
  }
  
  delay(10);
}

void WebsocketHandler::openWebSocket() {
  Serial.println("[ws] Opening websocket ports...");
  webSocketServer->begin();
}

bool WebsocketHandler::handleWebsocketTimeout(int clientID) {
  // Serial.println("[ws] handleWebsocketTimeout");
  
  if( wsHandler->getWebsocketFlag(clientID) == false 
      && wsHandler->getWebsocketPingReceived(clientID) == false ) {
    return true;
  }
  return false;
}

void WebsocketHandler::websocketTimeoutEvent() {
  wsHandler->websocketTimedOut();
}

void WebsocketHandler::websocketTimedOut() {
  //Serial.println("[ws] [timer] Check for connection timeout ...");
  
  for(int i = 0; i < MAXCLIENTS; i++) {
    if(webSocketConnections[i] != NULL) {
      Serial.print("[ws-timer] Client ");
      Serial.print(i);
      Serial.println(":");
      
      Serial.print("[ws-timer] Flag: ");
      Serial.println(wsHandler->getWebsocketFlag(i));
      Serial.print("[ws-timer] Ping: ");
      Serial.println(wsHandler->getWebsocketPingReceived(i));
    
      if(getWebsocketFlag(i) == true && getWebsocketPingReceived(i) == true) {
        // flag = true -> ping received
        setWebsocketPingReceived(i, true);
        // reset flag
        setWebsocketFlag(i, false);
      } else if (getWebsocketFlag(i) == false && getWebsocketPingReceived(i) == true) {
        // flag = false -> ping not received
        setWebsocketPingReceived(i, false);
        // reset flag
        setWebsocketFlag(i, false);
      } 
    } else {
      // initial state
      setWebsocketPingReceived(i, false);
      setWebsocketFlag(i, false);
    }
  }
}

void WebsocketHandler::setWebsocketFlag(int clientID, bool mStatus) {
  accessFlag.lock();
  
  websocketConnectionStatus[clientID*2] = mStatus;
  
  accessFlag.unlock();
}

bool WebsocketHandler::getWebsocketFlag(int clientID) {
  accessFlag.lock();

  bool returnParam = websocketConnectionStatus[clientID*2];

  accessFlag.unlock();
  
  return returnParam;
}

void WebsocketHandler::setWebsocketPingReceived(int clientID, bool mStatus) {
  accessFlag.lock();
  
  websocketConnectionStatus[clientID*2 + 1] = mStatus;

  accessFlag.unlock();
}

bool WebsocketHandler::getWebsocketPingReceived(int clientID) {
  accessFlag.lock();
  
  bool returnParam = websocketConnectionStatus[clientID*2 + 1];

  accessFlag.unlock();

  return returnParam;
}
