#pragma once
#include <ESP8266WiFi.h>

bool isRestart = false;

void restart() {
  isRestart = true;
  ESP.restart();
}