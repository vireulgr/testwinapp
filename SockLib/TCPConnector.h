#ifndef _TCPCONNECTOR_H_
#define _TCPCONNECTOR_H_
#include <exception>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <WinSock2.h>
#elif defined( _GNUG_ )
#include <sys/types.h>
#include <sys/socket.h>
#endif

class TCPStream;

class TCPConnector {

public:
    TCPStream * connect( int port, char const * addr ) throw (std::exception);
private:
    int resolveHost( char const * host, struct sockaddr_in * in_addr );
};

#endif

