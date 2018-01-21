#ifndef _TCPSTREAM_H_
#define _TCPSTREAM_H_
#include <winsock2.h>
#include <string>

// class TCPConnector;
// class TCPAcceptor;

class TCPStream {
    SOCKET      m_sock;
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
    TCPStream( SOCKET, struct sockaddr_in * addr );
    ~TCPStream( );
};

#endif

