#pragma once
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <vector>

static std::vector<AsyncClient*> clients;
