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
// `cp ESP8266/Consts.h.template ESP8266/Consts.h` to create
#include "Consts.h"
#include <MD5Builder.h>
#include "Utils.h"
#include "EspController.h"
#include "WsEventDispatcher.h"
#include "SerialEventDispatcher.h"

#define SOFT_AP 1
#define DNS_SERVER 0
#define WDT_TIMEOUT_MS 30000


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);

#if DNS_SERVER && SOFT_AP
    DNSServer dnsServer;
#endif

ESP8266WebServer httpServer(80);
void handleRoot();

WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

int counter = 0;
Print * debug = 0;
WsEventDispatcher wsEventDispatcher(&webSocket, debug);
SerialEventDispatcher serialEventDispatcher(&Serial, debug);
EspController controller(&wsEventDispatcher, &serialEventDispatcher, debug);

void setup() {
    ESP.wdtEnable(WDT_TIMEOUT_MS);
    Serial.begin(9600);
    Serial.println("start");
    SPIFFS.begin();
    delay(1000);

#if SOFT_AP
    Serial.println("Configuring access point...");
    const char *ssid = SOFT_AP_SSID;
    const char *password = SOFT_AP_PASSWORD;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");    Serial.println(myIP);
#else
    const char *ssid = MY_WIFI_SSID;
    const char *password = MY_WIFI_PASSWORD;
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nLocal IP address: "); Serial.println(WiFi.localIP());
#endif

#if DNS_SERVER && SOFT_AP
    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNS_PORT, "ardu.local", apIP);
#endif


    httpServer.on("/status", handleRoot);
    httpServer.serveStatic("/", SPIFFS, "/", "max-age=86400;");
    httpServer.begin();
    Serial.println("HTTP server started");

    controller.setup();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket server started");
}

void loop() {
    ESP.wdtEnable(WDT_TIMEOUT_MS);
#if DNS_SERVER && SOFT_AP
    dnsServer.processNextRequest();
    yield();
#endif

    httpServer.handleClient();
    yield();

    webSocket.loop();
    yield();
    serialEventDispatcher.loop();
    yield();
    controller.loop();
}

void webSocketEvent(uint8_t num,
                    WStype_t type,
                    uint8_t * payload,
                    size_t lenght) {
    wsEventDispatcher.webSocketEvent(num, type, payload, lenght);
}

void handleRoot() {
    ls();

    counter += 1;
    httpServer.send(200, "text/html", "<h1>Hello World " + String(counter) + "</h1>");
}

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
