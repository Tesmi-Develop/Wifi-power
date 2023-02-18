#include <ESPAsyncWebServer.h>

class WebRequestHandler {
  public:
    String url;
    WebRequestMethod method;
    ArRequestHandlerFunction callback;
    ArUploadHandlerFunction onUpload;
    ArBodyHandlerFunction onBody;

    WebRequestHandler(String url, WebRequestMethod method, ArRequestHandlerFunction callback) {
      this->url = url;
      this->method = method;
      this->callback = callback;
    }

    WebRequestHandler(String url, WebRequestMethod method, ArRequestHandlerFunction callback, ArUploadHandlerFunction onUpload, ArBodyHandlerFunction onBody) {
      this->url = url;
      this->method = method;
      this->callback = callback;
      this->onUpload = onUpload;
      this->onBody = onBody;
    }
};