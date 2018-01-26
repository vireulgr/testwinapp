#ifndef _TCPSTREAM_H_
#define _TCPSTREAM_H_

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock2.h>
#elif defined( _GNUG_ )
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <string>

// class TCPConnector;
// class TCPAcceptor;

class TCPStream {
#if defined( _WIN32 ) || defined( _WIN64 )
    SOCKET      m_sock;
#elif defined( _GNUG_ )
    int         m_sock;
#endif
    std::string m_addr;
    int         m_port;

    friend class TCPConnector;
    friend class TCPAcceptor;

public:
    std::string getAddress();
    int getPort();

    int write( char const * buf, int bufLen );
    int read( char * buf, int bufLen );
private:
#if defined( _WIN32 ) || defined( _WIN64 )
    TCPStream( SOCKET, struct sockaddr_in * addr );
#elif defined( _GNUG_ )
    TCPStream( int, struct sockaddr_in * addr );
#endif
    ~TCPStream( );
};

#endif

