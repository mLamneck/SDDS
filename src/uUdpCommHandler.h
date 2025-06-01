#ifndef UUDPCOMMHANDLER_H
#define UUDPCOMMHANDLER_H

#include <Ws2tcpip.h>
#include <winsock2.h>
#include "uMultask.h"
#include "uStream.h"
#include "uPlainCommHandler.h"

class TerrorCodeToString{
    char Fbuffer[254];
    public:
        TerrorCodeToString(){Fbuffer[0] = '\0';}

        char* codeToString(int _code){
            FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
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

typedef dtypes::int16 TudpPort; 

class Tudp : public TstringStream{

    private:
        WSADATA Fwsa;
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
            local.sin_port = htons(_localPort);
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
            int fromLen = sizeof(Fsrc);
            Flength = recvfrom(Fs,&Fbuffer[0],sizeof(Fbuffer)-1,0,(sockaddr*)&Fsrc,&fromLen);
            if (Flength > 0){
                Fbuffer[Flength] = '\0';
                return true;
            }
            return false;
        }

		void send(const char* _msg, int _len = -1) {
			if (_len < 1) {
				_len = strlen(_msg);
			}

			const size_t maxUDPPacket = 32000;
			size_t sent = 0;

			Fdest.sin_port = htons(FremotePort);

			while (sent < (size_t)_len) {
				size_t remaining = _len - sent;
				size_t chunkSize = std::min(maxUDPPacket, remaining);
				bool isLast = (sent + chunkSize >= (size_t)_len);

				std::string chunk(_msg + sent, chunkSize);
				if (isLast) {
					chunk += '\n';
				}

				//debug::log("UDP: sending %s", chunk.c_str());
				int ret = sendto(FsendSock, chunk.c_str(), (int)chunk.size(), 0, (sockaddr*)&Fdest, sizeof(Fdest));
				if (ret == SOCKET_ERROR) {
					int errorCode = WSAGetLastError();
					debug::log("error in send: WSAGetLastError=%d", errorCode);
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
        };
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

#endif //UUDP_H
