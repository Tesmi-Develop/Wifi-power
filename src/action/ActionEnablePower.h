#include <ESP8266WiFi.h>
#include <Action.h>
#include <config.h>
#include <send.h>

class ActionEnablePower : Action {
    public:
        ActionEnablePower(int time) : Action(time) {
            _id = ENABLE_POWER;
        }

        static ActionEnablePower* Create(int time) {
            ActionEnablePower* instance = new ActionEnablePower(time);
            Action::addAction(instance);
            return instance;
        }

        void callback() override {
            Serial.println("Enable!");
            digitalWrite(PIN_RELAY, HIGH);

            DynamicJsonDocument dataForSend(1024);
            dataForSend["value"] = digitalRead(PIN_RELAY);

            sendPackageAllClients(GET_POWER, dataForSend);
        }
};