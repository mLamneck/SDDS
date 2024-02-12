#include "uPlatform.h"

/*  WebServer/socket not implemented for debug platforms
    toDo: find a solution that works on windows/linux...
*/
#if MARKI_DEBUG_PLATFORM != 1

#include "uWebCommHandler.h"
#include "site_browser.h"

void __onEvent(AsyncWebSocket *socket, AsyncWebSocketClient *client, AwsEventType type,
    void *arg, uint8_t *data, size_t len) {
      static_cast<TwebSocket*>(socket)->_onEvent(client,type,arg,data,len);
}

void TwebSocket::init(TwebServer& _server){
  AsyncWebSocket::onEvent(__onEvent);
  _server.addHandler(this);
}

void TwebCommHandler::begin(){
  /*  the following can be used if we somewhen need to derive TwebSocket from something else than AsyncWebSocket.
   *  For the moment we stick to the __onEvent function as it has less overhead because we don't rely on functionals

  FwebSocket.onEvent([this](AsyncWebSocket *socket, AsyncWebSocketClient *client, AwsEventType type,void *arg, uint8_t *data, size_t len){
    FwebSocket._onEvent(client,type,arg,data,len);
  });
  */
  FwebSocket.init(FwebServer);
  
  FwebServer.route("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", site_browser, sizeof(site_browser));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });  

  FwebServer.begin();

}

#endif
