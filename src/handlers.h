#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include "config.h"
#include <Handler.h>
#include <send.h>
#include <vector>
#include <action/ActionEnablePower.h>
#include <action/ActionDisablePower.h>

static std::vector<Handler*> handlers = {
  new Handler(GET_POWER, [](AsyncClient *client, DynamicJsonDocument data) {
    if (client->canSend()) {
      DynamicJsonDocument packageForSend(1024);
      DynamicJsonDocument dataForSend(1024);
      String packageForSendString;

      dataForSend["value"] = digitalRead(PIN_RELAY);

      packageForSend["id"] = GET_POWER;
      packageForSend["data"] = dataForSend;

      serializeJson(packageForSend, packageForSendString);
      client->add(packageForSendString.c_str(), packageForSendString.length());
      client->send();
    }
  }),
  new Handler(SET_POWER, [](AsyncClient *client, DynamicJsonDocument data) {
    if (!data["data"].containsKey("value")) {
      Serial.println("Error in handler 'SET_POWER': not founded value");
      return;
    }
    if (!data["data"]["value"].is<bool>()) {
      Serial.println("Error in handler 'SET_POWER': value is not type bool");
      return;
    }

    digitalWrite(PIN_RELAY, data["data"]["value"].as<bool>());
  }),
  new Handler(SWITCH_POWER, [](AsyncClient *client, DynamicJsonDocument data) {
    bool state = digitalRead(PIN_RELAY);
    digitalWrite(PIN_RELAY, !state);

    DynamicJsonDocument dataForSend(1024);
    dataForSend["value"] = !state;

    sendPackageAllClients(GET_POWER, dataForSend);
  }),
  new Handler(GET_TASK_LIST, [](AsyncClient *client, DynamicJsonDocument package) {
    if (client->canSend()) {
      DynamicJsonDocument packageForSend(1024);
      DynamicJsonDocument dataForSend(1024);
      DynamicJsonDocument docTasks(1024);
      String packageForSendString;

      JsonArray tasks = docTasks.to<JsonArray>();

      for (Action *action : Action::actions) {
        DynamicJsonDocument jsonAction(1024);
        Serial.println(ENABLE_POWER);
        jsonAction["id"] = action->_id;
        jsonAction["time"] = (action->time - (action->currentMillis - action->startingMillis)) / 1000;

        tasks.add(jsonAction);
      }

      dataForSend["tasks"] = tasks;

      packageForSend["id"] = GET_TASK_LIST;
      packageForSend["data"] = dataForSend;

      serializeJson(packageForSend, packageForSendString);
      Serial.println(packageForSendString.c_str());
      client->add(packageForSendString.c_str(), packageForSendString.length());
      client->send();
    }
  }),
  new Handler(ADD_ACTION_ENABLE_POWER, [](AsyncClient *client, DynamicJsonDocument data) {
    if (!data["data"].containsKey("time")) {
      Serial.println("Error in handler 'ADD_ACTION_ENABLE_POWER': not founded time");
      return;
    }

    if (!data["data"]["time"].is<int>()) {
      Serial.println("Error in handler 'ADD_ACTION_ENABLE_POWER': time is not type int");
      return;
    }

    ActionEnablePower::Create(data["data"]["time"]);
  }),
  new Handler(ADD_ACTION_DISABLE_POWER, [](AsyncClient *client, DynamicJsonDocument data) {
    if (!data["data"].containsKey("time")) {
      Serial.println("Error in handler 'ADD_ACTION_DISABLE_POWER': not founded time");
      return;
    }

    if (!data["data"]["time"].is<int>()) {
      Serial.println("Error in handler 'ADD_ACTION_DISABLE_POWER': time is not type int");
      return;
    }
    
    ActionDisablePower::Create(data["data"]["time"]);
  }),
};

Handler* findHandlerById(int id) {
  for (Handler *templateHandler : handlers) {
    if (templateHandler->id == id) {
      return templateHandler;
    }
  }

  return NULL;
}
