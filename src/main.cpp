#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"
#include <ESPAsyncTCP.h>
#include <vector>
#include <ArduinoJson.h>
#include "handlers.h"
#include <HandlerId.h>
#include <typeinfo>
#include <clients.h>
#include <Action.h>
#include <Data/Data.h>
#include <web/web.h>
#include <restart.h>

static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
	char dataString[len + 1];
	memcpy(dataString, (uint8_t*)data, len);

	Serial.printf("\n data received from client %s \n", client->remoteIP().toString().c_str());

  DynamicJsonDocument dataJson(1024);
  deserializeJson(dataJson, dataString);
  
  if (!dataJson.containsKey("id") || !dataJson.containsKey("data")) {
    Serial.println("Incorrect package: not founded id or data");
    return;
  }

  Handler *handler = findHandlerById(dataJson["id"]);

  if (handler == NULL) {
    Serial.println("Incorrect package: not founded handler for this package");
    return;
  }

  if (!dataJson["data"].is<JsonObject>()) {
    Serial.println("Incorrect package: data is not object");
    return;
  }

  handler->callback(client, dataJson);
}

static void handleError(void* arg, AsyncClient* client, int8_t error) {
  Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
}

static void handleDisconnect(void* arg, AsyncClient* client) {
	Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
}

static void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) {
	Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}

static void handleNewClient(void* arg, AsyncClient* client) {
	Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());

	clients.push_back(client);

	client->onData(&handleData, NULL);
	client->onError(&handleError, NULL);
	client->onDisconnect(&handleDisconnect, NULL);
	client->onTimeout(&handleTimeOut, NULL);
}

void ICACHE_RAM_ATTR onReset() {
  if (isRestart) { return; }
  Serial.println("Reset");
  dataReset();
  dataSave();
  restart();
}

void createNetwork() {
  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(APSSID, APPSK);
  IPAddress apip = WiFi.softAPIP(); 
  Serial.print("AP IP address: ");
  Serial.println(apip);
}

void connectToNetwork() {
  Serial.println("SSID: " + EEPROMData.SSID);
  Serial.println("Password: " + EEPROMData.Password);
  WiFi.begin(EEPROMData.SSID, EEPROMData.Password);
  Serial.print("Connecting to WiFi.");

  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      digitalWrite(PIN_LEG_STATE, !(bool)digitalRead(PIN_LEG_STATE));
      Serial.print(".");
  }
  digitalWrite(PIN_LEG_STATE, HIGH);
  Serial.println("");
  Serial.println(WiFi.localIP());
}

void initPins() {
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LEG_STATE, OUTPUT);
  pinMode(PIN_RESET_DATA, INPUT_PULLUP);
  attachInterrupt(PIN_RESET_DATA, onReset, FALLING);

  digitalWrite(PIN_LEG_STATE, LOW);
  Serial.println("Pins inited!");
}

void setup() {
	Serial.begin(9600);

  initPins();
  dataInit();
  dataGet(); // pull the date from the eeprom

  if (EEPROMData.SSID != "" && EEPROMData.Password != "") {
    Serial.println("Run mode wifi");
	  connectToNetwork();
  } else {
    createNetwork();
    initWeb();
  }

	auto* server = new AsyncServer(PORT);
	server->onClient(&handleNewClient, server);
	server->begin();
}

void loop() {
  if (Action::actions.size() != 0) {
    Action::actions[0]->updateTime();
  }
}
