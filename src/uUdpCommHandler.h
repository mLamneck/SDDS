#ifndef UUDPCOMMHANDLER_H
#define UUDPCOMMHANDLER_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <string.h>

    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    typedef int SOCKET;
#endif

#include <algorithm>
#include <string>

#include "uMultask.h"
#include "uStream.h"
#include "uPlainCommHandler.h"

class TerrorCodeToString{
    char Fbuffer[254];
public:
    TerrorCodeToString(){Fbuffer[0] = '\0';}

    char* codeToString(int _code){
#ifdef _WIN32
        FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            _code,
            MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
            Fbuffer,
            sizeof (Fbuffer),
            NULL);
#else
        strncpy(Fbuffer, strerror(_code), sizeof(Fbuffer));
#endif
        return &Fbuffer[0];
    }

    char* lastError(){
#ifdef _WIN32
        return codeToString(WSAGetLastError());
#else
        return codeToString(errno);
#endif
    }
};

typedef dtypes::int16 TudpPort;

class Tudp : public TstringStream{

private:
#ifdef _WIN32
    WSADATA Fwsa;
#endif

    SOCKET Fs;
    SOCKET FsendSock;
    sockaddr_in Fdest;
    sockaddr_in Fsrc;

    TudpPort FremotePort;

    bool Fready;
    int Flength;
    char Fbuffer[64];

public:

    Tudp(TudpPort _localPort = 9501, TudpPort _remotePort = 9500){
        FremotePort = _remotePort;

        sockaddr_in local;

        Fready = false;

#ifdef _WIN32
        if (WSAStartup(MAKEWORD(2,2),&Fwsa) != 0){ return; }
#endif

        if((Fs = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET) { return; }
        if((FsendSock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET) { return; }

#ifdef _WIN32
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        setsockopt(FsendSock, SOL_SOCKET, SO_SNDTIMEO,(const char*)&tv,sizeof(tv));
#endif

        local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port = htons(_localPort);

        if (bind(Fs, (sockaddr *) &local, sizeof(local)) != 0) { return; }

#ifdef _WIN32
        ULONG NonBlock = 1;
        if (ioctlsocket(Fs, FIONBIO, &NonBlock) == SOCKET_ERROR){ return; }
#else
        int flags = fcntl(Fs, F_GETFL, 0);
        fcntl(Fs, F_SETFL, flags | O_NONBLOCK);
#endif

        Fdest.sin_family = AF_INET;
        Fdest.sin_addr.s_addr = inet_addr("127.0.0.1");

        Fready = true;
    };

    ~Tudp(){
#ifdef _WIN32
        closesocket(Fs);
        closesocket(FsendSock);
        WSACleanup();
#else
        close(Fs);
        close(FsendSock);
#endif
    }

    char* data(){ return &Fbuffer[0]; }

    const int length() { return Flength; }

    bool receive(){
        socklen_t fromLen = sizeof(Fsrc);

        Flength = recvfrom(Fs,&Fbuffer[0],sizeof(Fbuffer)-1,0,(sockaddr*)&Fsrc,&fromLen);

        if (Flength > 0){
            Fbuffer[Flength] = '\0';
            return true;
        }

        return false;
    }

    void send(const char* _msg, int _len = -1) {

        if (_len < 1){
            _len = strlen(_msg);
        }

        const size_t maxUDPPacket = 32000;
        size_t sent = 0;

        Fdest.sin_port = htons(FremotePort);

        while (sent < (size_t)_len){

            size_t remaining = _len - sent;
            size_t chunkSize = std::min(maxUDPPacket, remaining);
            bool isLast = (sent + chunkSize >= (size_t)_len);

            std::string chunk(_msg + sent, chunkSize);

            if (isLast){
                chunk += '\n';
            }

            int ret = sendto(FsendSock, chunk.c_str(), (int)chunk.size(), 0, (sockaddr*)&Fdest, sizeof(Fdest));

            if (ret == SOCKET_ERROR){

#ifdef _WIN32
                int errorCode = WSAGetLastError();
#else
                int errorCode = errno;
#endif

                debug::log("error in send: code=%d", errorCode);
                break;
            }

            sent += chunkSize;
        }
    }

    void flush() override {
        send(TstringStream::data());
        clear();
    }
};

class TudpCommHandler : public Tthread{

    Tudp Fudp;
    TplainCommHandler FcommHandler;

    void execute(Tevent* _ev){

        while (Fudp.receive()){
            FcommHandler.handleMessage(Fudp.data(),Fudp.length());
        }

        setTimeEvent(10);
    }

public:

    void handleMessage(const char* _msg) {
        FcommHandler.handleMessage(_msg);
    }

    TudpCommHandler(TmenuHandle& _root):
        FcommHandler(&_root,&Fudp)
    {
    }
};

typedef TudpCommHandler TudpSpike;

#endif