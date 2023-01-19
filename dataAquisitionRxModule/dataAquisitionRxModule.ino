/*
 * Rx Module transmits aquired data to the Rx Module via LoRa
*/

// Include libraries
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// LoRa define pins
#define ss 5
#define rst 17
#define dio0 2

// WiFi credentials
#define WIFI_SSID       "TP-Link_C400"
#define WIFI_PASSWORD   "25531497"

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Server
String website = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The random number is: <span id='rand'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { Socket.send('Sending back some random stuff'); } function processCommand(event) { document.getElementById('rand').innerHTML = event.data; console.log(event.data); } window.onload = function(event) { init(); }</script></html>";
// Need more testing
// String website = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Potentiometer measurements [Volts]</h1><p>Measured value: <span id='rand'>-</span></p><span style='color: #003366;'><h1>GPS measurements [Volts]</h1><p>Measured value: <span id='rand'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { Socket.send('Sending back some random stuff'); } function processCommand(event) { document.getElementById('rand').innerHTML = event.data; console.log(event.data); } window.onload = function(event) { init(); }</script></html>";


int randomval = random(100);
int interval = 5000; // virtual delay
unsigned long previousMillis = 0; // Tracks the time since last event fired
char receivedVoltage = 0;
char receivedVoltageArray[20] = "";

int counter = 0;

void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("LoRa Receiver");

  // ===========================================
  // ============= Setup LoRa ==================
  // ===========================================
  LoRa.setPins(ss, rst, dio0);

  //866E6 for Europe
  while (!LoRa.begin(866E6)) 
  {
    Serial.println(".");
    delay(500);
  }
   
  // Change sync word (0xF3) to match the receiver. Ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  LoRa.onReceive(onReceive);
  LoRa.receive();
  
  Serial.println("LoRa Initializing OK!");

  // ===========================================
  // ============== Setup WiFi =================
  // ===========================================
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    // Serial.println("WiFi not connected...");
    delay(200);
  }

  Serial.println("WiFi connected!");
  Serial.print("The IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text\html", website);
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() 
{
  server.handleClient();  // webserver methode that handles all Client
  webSocket.loop();

  String str = String(receivedVoltage);
  // int str_len = str.length() + 1;
  // char char_array[str_len];
  // str.toCharArray(char_array, str_len);
  webSocket.broadcastTXT(receivedVoltageArray);
  
  delay(200);
}

void webSocketEvent (byte num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Client " + String(num) + " connected");
      break;
    case WStype_TEXT:
      for (int i=0; i<length; i++) {                  // print received data from client
        Serial.print((char)payload[i]);
      }
      Serial.println("");
      break;
    default:
      Serial.println("default");
      break;
  }
}

void onReceive(int packetSize) {
  // received a packet
  Serial.print("Received packet '");

  // read packet
  for (int i = 0; i < packetSize; i++) {
    receivedVoltage = (char)LoRa.read();
    // Serial.print(receivedVoltage);
    receivedVoltageArray[i] = receivedVoltage;
  }

  Serial.println(receivedVoltageArray);

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

