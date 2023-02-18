#include <LittleFS.h>
#include <web/WebRequestHandler.h>
#include <web/utility.h>
#include <map>
#include <restart.h>

AsyncWebServer webServer(80);

std::vector<WebRequestHandler*> captiveRequestHandlers = {
  new WebRequestHandler("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Sent html");
    request->send(LittleFS, "/index.html", String());
  }),
  new WebRequestHandler("/applySettings", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    Serial.println("new settings");

    char dataString[len + 1];
    memcpy(dataString, data, len);

    DynamicJsonDocument dataJson(1024);
    deserializeJson(dataJson, dataString);

    if (!dataJson.containsKey("SSID") || !dataJson.containsKey("Password")) {
      Serial.println("Incorrect HTTP_POST /applySettings: not founded SSID or Password");
      return;
    }

    if (!dataJson["SSID"].is<String>() || !dataJson["Password"].is<String>()) {
      Serial.println("Incorrect HTTP_POST /applySettings: SSID or Password is not a string");
      return;
    }

    EEPROMData.SSID = dataJson["SSID"].as<String>();
    EEPROMData.Password = dataJson["Password"].as<String>();
    dataSave();
    Serial.println("Save");
    request->send(200);
    ESP.restart();
  })
};

std::string getContentType(String url) {
  typedef std::function<void()> CallbackContentType;

  std::vector<std::string> pathSplit = split(url.c_str(), '.');
  std::string extension = pathSplit.back();
  std::string contentType;
  
  std::map<std::string, CallbackContentType> contentTypes = {
    {"ico", [&](){
      contentType = "image/x-img";
    }},
    {"css", [&](){
      contentType = "text/css";
    }},
    {"js", [&](){
      contentType = "text/javascript";
    }},
    {"html", [&](){
      contentType = "text/html";
    }},
  };

  if (contentTypes.find(extension) == contentTypes.cend()) {
    return "text";
  }

  contentTypes[extension]();

  return contentType;
}

void addRequestHandlerForSendFiles(std::string path = "/website") {
  File root = LittleFS.open(path.c_str(), "r");
  File file = root.openNextFile();

  while (file)
  {
    if (file.isDirectory()) {
      addRequestHandlerForSendFiles(file.fullName());
      file = root.openNextFile();
      continue;
    }

    Serial.print("FILE: ");
    Serial.println(file.fullName());

    captiveRequestHandlers.push_back(new WebRequestHandler(String("/") + String(file.name()), HTTP_GET, [file](AsyncWebServerRequest *request) {
      Serial.println("New request: " + String(file.name()));
      request->send(LittleFS, file.fullName(), getContentType(file.name()).c_str());

    }));

    file = root.openNextFile();
  }
}

void initFileSystem() {
  if(!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void initWebRequestHandlers() {
  for (WebRequestHandler *templateHandler : captiveRequestHandlers) {
    String url = templateHandler->url;

    webServer.on(url.c_str(), templateHandler->method, templateHandler->callback, templateHandler->onUpload, templateHandler->onBody);
    Serial.println("Init handler: " + url);
  }
}

void initWeb() {

    initFileSystem();
    addRequestHandlerForSendFiles();
    initWebRequestHandlers();

    webServer.begin();
}