// Write AT image:
// esptool.py -p `ls /dev/ttyA*` write_flash 0x00000 ~/Downloads/AT23SDK101-nocloud.bin
//
// Start server with AT image:
/*
AT+CWMODE=2
AT+RST
AT+CWSAP="ardu","arduino64",1,0
AT+CWLIF
AT+CIFSR 192.168.1.2

*/
// Upload statis assets using tool:
// https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md#uploading-files-to-file-system

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include "FS.h"

#define DNS_SERVER 1
#define HTTP_SERVER 1
#define WESOCKET_SERVER 1


/* Set these to your desired credentials. */
const char *ssid = "ardu";
const char *password = "arduino64";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);

#if DNS_SERVER
    DNSServer dnsServer;
#endif

#if HTTP_SERVER
    ESP8266WebServer httpServer(80);
    void handleRoot();
#endif

#if WESOCKET_SERVER
    WebSocketsServer webSocket = WebSocketsServer(81);
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);
#endif

int counter = 0;

void setup() {


    delay(1000);
    Serial.begin(115200);
    Serial.println();
    Serial.println("Configuring access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);

    SPIFFS.begin();

    #if DNS_SERVER
        dnsServer.setTTL(300);
        dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
        dnsServer.start(DNS_PORT, "ardu.local", apIP);
    #endif


    #if HTTP_SERVER
        httpServer.on("/", handleRoot);
        httpServer.begin();
        Serial.println("HTTP server started");
    #endif

    #if WESOCKET_SERVER
        webSocket.begin();
        webSocket.onEvent(webSocketEvent);
        Serial.println("WebSocket server started");
    #endif

  IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");    Serial.println(myIP);
}

void loop() {
    #if DNS_SERVER
        dnsServer.processNextRequest();
    #endif

    #if HTTP_SERVER
        httpServer.handleClient();
    #endif

    #if WESOCKET_SERVER
        webSocket.loop();
    #endif
}

#if WESOCKET_SERVER
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }
}
#endif

#if HTTP_SERVER
void handleRoot() {
    ls();

    counter += 1;
    File root = SPIFFS.open("/Index.html", "r");
    httpServer.send(200, "text/html", root.readString());
    root.close();
}
#endif

void ls() {
    String str = "";
    Serial.println("ls:\n");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        str += dir.fileName();
        str += " / ";
        str += dir.fileSize();
        str += "\r\n";
    }
    Serial.print(str);
    Serial.println("\n#ls end");
}
