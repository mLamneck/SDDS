#ifndef UUDP_H
#define UUDP_H

#include <Ws2tcpip.h>
#include <winsock2.h>
#include "uMultask.h"
#include "uStream.h"

class TerrorCodeToString{
    char Fbuffer[254];
    public:
        TerrorCodeToString(){Fbuffer[0] = '\0';}

        char* codeToString(int _code){
            FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
               NULL,                // lpsource
               _code,                 // message id
               MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),    // languageid
               Fbuffer,              // output buffer
               sizeof (Fbuffer),     // size of msgbuf, bytes
               NULL);
            return &Fbuffer[0];
        }

        char* lastError(){
            return codeToString(WSAGetLastError());
        }

};

class Tudp : public TstringStream{

    private:
        WSADATA Fwsa;
        SOCKET Fs;
        SOCKET FsendSock;
        sockaddr_in Fdest;
        sockaddr_in Fsrc;

        bool Fready;
        int Flength;
        char Fbuffer[64];
    public:
        Tudp(){
            //local variables
            sockaddr_in local;

            Fready = false;
            if (WSAStartup(MAKEWORD(2,2),&Fwsa) != 0){ return; }

            //create a socket
            if((Fs = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET) { return; }
            if((FsendSock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET) { return; }

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 100000;
            if (setsockopt(FsendSock, SOL_SOCKET, SO_SNDTIMEO,(const char*)&tv,sizeof(tv)) < 0) {
                perror("Error");
            }

            //bind socket to local port
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_port = htons(9501);
            if (bind(Fs, (SOCKADDR *) & local, sizeof (local)) != 0) { return; }

            //switch socket to noneblocking mode
            ULONG NonBlock = 1;
            if (ioctlsocket(Fs, FIONBIO, &NonBlock) == SOCKET_ERROR){ return; }

            Fdest.sin_family = AF_INET;
            Fdest.sin_addr.s_addr = inet_addr( "127.0.0.1" );

            Fready = true;
        };

        ~Tudp(){
            WSACleanup();
        }


        char* data(){ return &Fbuffer[0]; }
        const int length() { return Flength; }

        bool receive(){
            //memset(&Fbuffer[0],'\0',sizeof(Fbuffer));
            //int ret = recv(Fs,&Fbuffer[0],sizeof(Fbuffer)-1,0);
            int fromLen = sizeof(Fsrc);
            Flength = recvfrom(Fs,&Fbuffer[0],sizeof(Fbuffer)-1,0,(sockaddr*)&Fsrc,&fromLen);
            if (Flength > 0){
                Fbuffer[Flength] = '\0';
                return true;
            }
            return false;
        }

        void send(const char* _msg, int _len = -1){
            if (_len < 1){ _len = strlen(_msg); }
            Fdest.sin_port = htons(9500);
            debug::log("UDP: sending %s", _msg);
            int ret = sendto(FsendSock, _msg, _len, 0, (sockaddr*)&Fdest, sizeof(Fdest) );
            if (ret == SOCKET_ERROR){
                    printf("error in send\n");
            }
        }

        void flush() override {
            send(TstringStream::data());
            clear();
        }

};

typedef void (*TudpOnMessage)(Tudp* _udp);

class TudpThread : public Tthread{
    int Fcnt = 0;
    Tudp Fudp;
    TudpOnMessage FonMsg;

    void execute(Tevent* _ev){
        while (Fudp.receive()){
            //handle message here
            FonMsg(&Fudp);
        };
        setTimeEvent(10);
    }

    public:
        Tudp* stream(){ return &Fudp; };

        void onPacket(TudpOnMessage _on){
            FonMsg = _on;
        }

};

#endif //UUDP_H
