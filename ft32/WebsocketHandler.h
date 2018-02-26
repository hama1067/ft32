/*
 * WebsocketHandler.h
 *
 *  Created on: Okt 27, 2017
 *      Author: joseph
 *
 *  description:    
 */

#ifndef WEBSOCKETHANDLER_H_
#define WEBSOCKETHANDLER_H_

#define MAXCLIENTS 4
#include <WiFi.h>
#include "WebSocketServer.h"
#include "WebSocketConnection.h"
#include "Mutex.h"
#include "ft_ESP32_SHM.h"

class WebsocketHandler {
public:
    WebsocketHandler(SHM *pSHM);
    ~WebsocketHandler();

    /*
     * 
     */
    void sendWebsocketMessage(String msg);

     /*
      * 
      */
    bool addWebsocketClient(WiFiClient * pClient, WebSocketServer *nWebSocketServer);

    /*
     * 
     */
    bool removeWebsocketClient(WiFiClient * pClient);

    /*
     * 
     */
    int getClientID(WiFiClient * pClient);

    /*
     * 
     */
    void handleWebsocketRequests();

    /*
     * 
     */
    void openWebsocket();
    
private:
    int clientCount;
    WebSocketConnection **webSocketConnections;
    WiFiServer *webSocketServer;

    Mutex addClient, removeClient, sendData, clientID;

    SHM *mSHM;
};

#endif /* WEBSOCKETHANDLER_H_ */
