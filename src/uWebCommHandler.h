#ifndef UWEBCOMMHANDLER_H
#define UWEBCOMMHANDLER_H

#include "uPlainCommHandler.h"
#include "uWebServer.h"

class TwebSocketClientContext : public TstringStream{
  private:
    AsyncWebSocketClient* Fclient;
    TplainCommHandler FcommHandler;
  public:
    TwebSocketClientContext(TmenuHandle* _ds, TstringStreamBuffer& _buffer, 
      AsyncWebSocketClient* _client) 
      : TstringStream(&_buffer)
      , FcommHandler(_ds,this)
      , Fclient(_client)
    {
    }    

    bool isClient(AsyncWebSocketClient* _client) { return _client == Fclient; }
    bool hasSameIp(AsyncWebSocketClient* _client) { 
      if (!Fclient){
        Serial.println("TwebSocketClientContext.hasSameIp: Fclient=nil");
        return false;
      }
      return _client->remoteIP() == Fclient->remoteIP(); 
    }
    bool inUse() { return Fclient != nullptr; }
    
    void doOnDisconnect(){
      FcommHandler.shutdown();      //tell plainCommHanlder to shut the fuck up
      Fclient = nullptr;
    }

    void sendMessage(const char* _msg){
      if (Fclient) Fclient->text(_msg);
    }

    void doOnConnect(AsyncWebSocketClient* _client){
      //Serial.println("TwebSocketClientContext.doConnect");
      Fclient = _client;
      //tell plainCommHandler to manage stuff
    }

    void handleMessage(const char* _msg){
      FcommHandler.handleMessage(_msg);
    }

    void flush() override { 
      //Serial.println("Commhandler wants to send data");
      sendMessage(data());
      TstringStream::clear();
    }
};

typedef TwebSocketClientContext* TpWebSocketClientContext;

class TwebSocket : public AsyncWebSocket{
  private:
    static const int MAX_CLIENTS = 4;
    
    //shared buffer for all clients, as we know we use it one at a time
    TmenuHandle* Fds;
    TstringStreamBuffer Fbuffer;
    TwebSocketClientContext* FclientContexts[MAX_CLIENTS];

  public:
   void init(TwebServer& _server);
    // 
    TwebSocket(TmenuHandle& _ds, TwebServer& _server) 
      : AsyncWebSocket("/ws")
      , Fds(_ds)
    {
      for (auto i=0; i<MAX_CLIENTS; i++){ FclientContexts[i] = nullptr; }
      //init(_server);
      //Fbuffer = makeBuffer(1024);
    }

  private:
    TpWebSocketClientContext findClientCtx(AsyncWebSocketClient* _client){
      for (auto i=0; i<MAX_CLIENTS; i++){ 
        TwebSocketClientContext* ctx = FclientContexts[i];
        if (!ctx) continue; 
        if (ctx->isClient(_client)){
          return ctx;
        }
      }
      return nullptr;
    }

    TpWebSocketClientContext findClientCtxByIp(AsyncWebSocketClient* _client){      
      for (auto i=0; i<MAX_CLIENTS; i++){ 
        TwebSocketClientContext* ctx = FclientContexts[i];
        if (!ctx) continue; 
        if (ctx->hasSameIp(_client)){
          return ctx;
        }
      }
      return nullptr;
    }

    TpWebSocketClientContext rejectClient(AsyncWebSocketClient* _client, const char* _error){
      Serial.println(_error);
      _client->text(_error);
      _client->close();
      return nullptr;
    }

    TpWebSocketClientContext allocateClientContext(AsyncWebSocketClient* _client){
      for (auto i=0; i<MAX_CLIENTS; i++){ 
        TwebSocketClientContext* ctx = FclientContexts[i];
        
        //slot in context array available?
        if (!ctx) {
          ctx = new TwebSocketClientContext(Fds,Fbuffer,_client);
          if (ctx == nullptr) return rejectClient(_client,"E 100 no memory");
          FclientContexts[i]=ctx;
          return ctx;
        };

        //if curr ctx is not used then reuse it
        if (!ctx->inUse()) return ctx;
      }

      return rejectClient(_client,"E 100 maximum number of clients exeeded");
    }

    void doOnClientConnect(AsyncWebSocketClient* _client){
      //Serial.printf("TwebSocket.onEvent client #%u connected from %s\n", _client->id(), _client->remoteIP().toString().c_str());

      if (findClientCtx(_client)){
        //check the ip!?
        //Serial.println("client alredy exists....");
        return;
      }

      //allow to connect the same ip multiple times???
      if (findClientCtxByIp(_client)){
        //Serial.println("client with this ip alredy connected....");
        //return;
      }

      auto ctx = allocateClientContext(_client);
      if (ctx) ctx->doOnConnect(_client);
    }

    void doOnClientDisconnect(AsyncWebSocketClient* _client){
      //Serial.printf("TwebSocket.onEvent client #%u disconnected\n", _client->id());
      TpWebSocketClientContext ctx = findClientCtx(_client);
      if (ctx) ctx->doOnDisconnect();
    }

    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
      //Serial.println("->handleWebSocketMessage");
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        char* msg = (char*)data;
        //Serial.println(msg);
        auto ctx = findClientCtx(client);
        if (ctx) ctx->handleMessage(msg);
      }
    }

  public:
    void _onEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (!client) return;
		
		Serial.print(millis());
        Serial.print(" in WebSocket.onEvent taskName= ");
        Serial.println(pcTaskGetName( nullptr ));

        switch (type) {
          case WS_EVT_CONNECT:
            doOnClientConnect(client);
            break;
          case WS_EVT_DISCONNECT:
            doOnClientDisconnect(client);
            break;
          case WS_EVT_DATA:
            handleWebSocketMessage(client, arg, data, len);
            break;
          case WS_EVT_PONG:
          case WS_EVT_ERROR:
            break;
      }
    }
};

class TwebCommHandler : public Tthread{
  private:
    TwebServer FwebServer;
    TwebSocket FwebSocket;

  public:
    TwebCommHandler(TmenuHandle& _ds) 
      : FwebServer(80)
      , FwebSocket(_ds,FwebServer)
    {

    }

    void begin();

    void execute(Tevent* _ev) override{
      begin();
    }

};


#endif