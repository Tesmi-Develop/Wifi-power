#include <ESP8266WiFi.h>
#include <EEPROM.h>

struct Data
{
    String SSID = "";
    String Password = "";
};

Data EEPROMData;

void dataInit() {
    EEPROM.begin(1000);
}

void dataGet() {
    EEPROM.get(0, EEPROMData);
}

void dataSave() {
    EEPROM.put(0, EEPROMData);
    delay(1000);
    EEPROM.commit();
}

void dataReset() {
    EEPROMData.SSID = "";
    EEPROMData.Password = "";
}
