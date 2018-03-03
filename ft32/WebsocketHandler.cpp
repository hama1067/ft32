#include "WebsocketHandler.h"

extern WebsocketHandler *wsHandler;
extern SHM * ptrSHM;

void webSocketTask(void* params) {
  WiFiClient* nClient = (WiFiClient*) params;
  WebSocketServer* nWebSocketServer = new WebSocketServer();
  
  String *data = new String();
  String *payload = new String();
  String *identifier = new String();

  if(wsHandler->addWebSocketClient(nClient, nWebSocketServer)) {
    if(nClient->connected() && nWebSocketServer->handshake(*nClient)) {
      Serial.print("[ws] Handshake with client ");
      Serial.print(wsHandler->getClientID(nClient));
      Serial.println(" successfully done.");
      
      Serial.println("[ws] Establishing connection...");
      
      while (nClient->connected()) {
      
        *data = nWebSocketServer->getData();
        
        if (data->length() > 0) {
          /*Serial.print("msg from client ");
          Serial.print(wsHandler->getClientID(nClient));
          Serial.print(": ");
          Serial.print(*data);
          Serial.print(" , free heap space: ");
          Serial.println(ESP.getFreeHeap());
          nWebSocketServer->sendData(*data);*/

          int delimiterIndex = data->indexOf('/');
          *payload = data->substring(delimiterIndex + 1, data->length());
          *identifier = data->substring(0, delimiterIndex);
        
          if(*identifier == "send") {
            Serial.println("[ws] send");

            ptrSHM->webData.data = *payload;
            ptrSHM->webData.contentLength = (*payload).length();

            Serial.println("[ws] Payload: " + ptrSHM->webData.data);
            
            //nWebSocketServer->sendData("ready");
            wsHandler->sendWebSocketMessage("ready");
          } else if(*identifier == "start") {
            Serial.println("[ws] start");
            ptrSHM->commonStart = 1;
            ptrSHM->commonPause = 0;
            //ptrSHM->commonStopp = 0; //old

            //nWebSocketServer->sendData("running");
            wsHandler->sendWebSocketMessage("running");    
          } else if(*identifier == "pause") {
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
            Serial.println("[ws] stop");

            ptrSHM->commonStart = 0;
            ptrSHM->commonPause = 0;
            //ptrSHM->commonStopp = 1; //old

            //nWebSocketServer->sendData("stopped");
            wsHandler->sendWebSocketMessage("stopped");
          }
        }
      
        delay(10); // Delay needed for receiving the data correctly
      }
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
    }

    last = current;
  }
  vTaskDelete(NULL);
}

WebsocketHandler::WebsocketHandler(SHM *pSHM) {
  Serial.println();
  Serial.print("[ws] Joining SHM container with status ");
  mSHM=pSHM;
  Serial.println(mSHM->commonStart);
  
  addClient.unlock();
  removeClient.unlock();
  sendData.unlock();
  clientID.unlock();
  
  clientCount = 0;
  webSocketServer = new WiFiServer(90);

  webSocketConnections = new WebSocketConnection*[MAXCLIENTS];

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

int WebsocketHandler::getClientID(WiFiClient * pClient) {
  clientID.lock();
  
  for(int i = 0; i < MAXCLIENTS; i++) {
    if(webSocketConnections[i]->pClient == pClient) {
      clientID.unlock();
      return i;
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
  
  delay(100);
}

void WebsocketHandler::openWebSocket() {
  Serial.println("[ws] Opening websocket ports...");
  webSocketServer->begin();
}

