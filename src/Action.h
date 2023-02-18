#pragma once
#include <ESP8266WiFi.h>
#include <vector>

enum ActionId {
  ENABLE_POWER, //0
  DISABLE_POWER, //1
};

class Action {
    public:
        static std::vector<Action*> actions;

        static void addAction(Action* action) {
            Action::actions.push_back(action);
            Serial.println("Added new action");
        }

        ActionId _id;
        int time;
        int startingMillis = -1;
        int currentMillis = -1;

        Action(int time) {
            this->time = time * 1000;
        }

        int findSelf() {
            auto actions = Action::actions;

            for (size_t i = 0; i < actions.size(); i++)
            {
                if (actions[i] == this) {
                    return i;
                }
            }
            
            return -1;
        }

        virtual void callback() {
            Serial.println("Not overwrite");
        };

        void updateTime() {
            if (startingMillis == -1) {
                this->startingMillis = millis();
            }

            this->currentMillis = millis();

            if (this->currentMillis - this->startingMillis >= this->time) {
                this->callback();

                int position = findSelf();
                if (position == -1) { return; }
                
                Action::actions.erase(Action::actions.begin() + position);
            }
        }
};

std::vector<Action*> Action::actions = {};