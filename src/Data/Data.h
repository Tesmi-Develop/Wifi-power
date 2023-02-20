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
    std::string ssid;
    std::string password;

    int len1 = EEPROM.read(0);
    for (byte i = 1; i < len1 + 1; i++)
    {
        ssid += EEPROM.read(i);
    }

    int len2 = EEPROM.read(len1 + 1);
    for (byte i = len1 + 2; i < len1 + 2 + len2; i++)
    {
        password += EEPROM.read(i);
    }

    EEPROMData.SSID = String(ssid.c_str());
    EEPROMData.Password = String(password.c_str());
}

void dataSave() {
    int len1 =  EEPROMData.SSID.length();
    EEPROM.write(0, len1);
    for (byte i = 1; i < len1 + 1; i++)
    {
        EEPROM.write(i, EEPROMData.SSID[i - 1]);
    }

    int len2 = EEPROMData.Password.length();
    EEPROM.write(len1 + 1, len2);
    byte a = 0;
    for (byte i = len1 + 2; i < len1 + 2 + len2; i++)
    {
        EEPROM.write(i, EEPROMData.Password[a]);
        a += 1;
    }

    delay(1000);
    EEPROM.commit();
}

void dataReset() {
    EEPROMData.SSID = "";
    EEPROMData.Password = "";

    for (size_t i = 0; i < EEPROM.length(); i ++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}
