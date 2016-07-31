#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);
void handleRoot();

/* Set these to your desired credentials. */
const char *ssid = "ardu";
const char *password = "arduino64";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);

DNSServer dnsServer;
ESP8266WebServer httpServer(80);
#define USE_SERIAL Serial
WebSocketsServer webSocket = WebSocketsServer(81);

int counter = 0;

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(ssid, password);

  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, "ardu.local", apIP);


	httpServer.on("/", handleRoot);
	httpServer.begin();
	Serial.println("HTTP server started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");

  IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");	Serial.println(myIP);
}

void loop() {
  dnsServer.processNextRequest();
	httpServer.handleClient();
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

				// send message to client
				webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }
}

void handleRoot() {
  counter += 1;
	//httpServer.send(200, "text/html", "<h1>You are connected, counter=" + String(counter) + "</h1>");
  httpServer.send(200, "\
    text/html", "<html><head><script>\
    var connection = new WebSocket('ws://'+location.hostname+':81/',\
    ['arduino']);connection.onopen = function () {\
    connection.send('Connect ' + new Date()); }; connection.onerror =\
    function (error) {    console.log('WebSocket Error ', error);};\
    connection.onmessage = function (e) {  console.log('Server: ', e.data);};\
    function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);\
    var g = parseInt(document.getElementById('g').value).toString(16);\
    var b = parseInt(document.getElementById('b').value).toString(16);\
    if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }\
    if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;\
    console.log('RGB: ' + rgb); connection.send(rgb); }</script>\
    </head><body>LED Control:<br/><br/>R:\
    <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\"\
    onchange=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\"\
    min=\"0\" max=\"255\" step=\"1\" onchange=\"sendRGB();\" /><br/>B:\
    <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\"\
    onchange=\"sendRGB();\" /><br/></body></html>");
}
