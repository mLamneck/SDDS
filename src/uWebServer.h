#ifndef UWEBSERVER_H
#define UWEBSERVER_H

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  //#include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h> 
  #include <ESPAsyncTCP.h>
#endif

#pragma push_macro("on")

  #undef on
  #include <ESPAsyncWebServer.h>

  class TwebServer : public AsyncWebServer{
    public:
      using AsyncWebServer::AsyncWebServer;

      //wrapper around AsyncWebServers on method, because it crashes with out handy on macro...
      //route is a better name anyway
      void route(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest){
        AsyncWebServer::on(uri,method,onRequest);
      }
  };

#pragma pop_macro("on")

#endif