#pragma one
#include <ESP8266WiFi.h>
#include <Action.h>
#include <config.h>
#include <send.h>

class ActionDisablePower : Action {
    public:
        ActionDisablePower(int time) : Action(time) {
            _id = DISABLE_POWER;
        }

        static ActionDisablePower* Create(int time) {
            ActionDisablePower* instance = new ActionDisablePower(time);
            Action::addAction(instance);
            return instance;
        }

        void callback() override {
            Serial.println("Disable!");
            digitalWrite(PIN_RELAY, LOW);

            DynamicJsonDocument dataForSend(1024);
            dataForSend["value"] = digitalRead(PIN_RELAY);

            sendPackageAllClients(GET_POWER, dataForSend);
        }
};