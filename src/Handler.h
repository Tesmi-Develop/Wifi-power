#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <HandlerId.h>

class Handler {
    public:
        typedef std::function<void(AsyncClient*, DynamicJsonDocument)> HandlerCallback;
        HandlerId id;
        HandlerCallback callback;

        Handler(HandlerId id, HandlerCallback callback) {
            this->id = id;
            this->callback = callback;
        }
};