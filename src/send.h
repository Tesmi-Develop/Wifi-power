#pragma once
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <HandlerId.h>
#include <ArduinoJson.h>
#include <clients.h>

static void sendPackage(AsyncClient *client, HandlerId id, DynamicJsonDocument dataForSend) {
  if (!client->canSend()) { return; }
  
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

static void sendTaskListAllClients() {
  DynamicJsonDocument dataForSend(1024);
  DynamicJsonDocument docTasks(1024);
  JsonArray tasks = docTasks.to<JsonArray>();

  for (Action *action : Action::actions) {
    DynamicJsonDocument jsonAction(1024);
    Serial.println(ENABLE_POWER);
    jsonAction["id"] = action->_id;
    jsonAction["time"] = (action->time - (action->currentMillis - action->startingMillis)) / 1000;

    tasks.add(jsonAction);
  }

  dataForSend["tasks"] = tasks;
  sendPackageAllClients(GET_TASK_LIST, dataForSend);
}

static void sendTaskList(AsyncClient *client) {
  DynamicJsonDocument dataForSend(1024);
  DynamicJsonDocument docTasks(1024);
  JsonArray tasks = docTasks.to<JsonArray>();

  for (Action *action : Action::actions) {
    DynamicJsonDocument jsonAction(1024);
    jsonAction["id"] = action->_id;
    jsonAction["time"] = (action->time - (action->currentMillis - action->startingMillis)) / 1000;

    tasks.add(jsonAction);
  }

  dataForSend["tasks"] = tasks;
  sendPackage(client, GET_TASK_LIST, dataForSend);
}
