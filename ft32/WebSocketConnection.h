#include "WebSocketServer.h"
#include "WiFi.h"

struct WebSocketConnection {
    WiFiClient *pClient;
    WebSocketServer *pWebSocketServer;

    WebSocketConnection(WiFiClient * nClient, WebSocketServer *nWebSocketServer) {
      pClient = nClient;
      pWebSocketServer = nWebSocketServer;
    }
};
