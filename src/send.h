#pragma once
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <HandlerId.h>
#include <ArduinoJson.h>
#include <clients.h>
#include <HandlerId.h>

static void sendPackage(AsyncClient *client, HandlerId id, DynamicJsonDocument dataForSend) {

  DynamicJsonDocument packageForSend(1024);
  String packageForSendString;

  packageForSend["id"] = id;
  packageForSend["data"] = dataForSend;

  serializeJson(packageForSend, packageForSendString);
  client->add(packageForSendString.c_str(), packageForSendString.length());
  client->send();
}

static void sendPackageAllClients(HandlerId id, DynamicJsonDocument dataForSend) {
  for (AsyncClient *client : clients) {
    sendPackage(client, id, dataForSend);
  }
}
