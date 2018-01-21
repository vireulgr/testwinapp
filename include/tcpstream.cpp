#include "tcpstream.h"

TCPStream::TCPStream( SOCKET sck, struct sockaddr_in * addr ) 
    : m_sock( sck )
{
    char ipAddr[50] = {0};
    inet_ntop( PF_INET, (struct in_addr*)&(addr->sin_addr.s_addr), ipAddr, sizeof(ipAddr)-1 );
    m_addr = ipAddr;
    m_port = ntohs( address->sin_port );
}

TCPStream::~TCPStream( ) {
    closesocket( m_sock );
}


std::string TCPStream::getAddress() {
    return m_addr;
}

int TCPStream::getPort() {
    return m_port;
}

int TCPStream::write( char const * buf, int bufLen ) {
    return send( m_sock, buf, bufLen, 0 );
}

int TCPStream::read( char * buf, int bufLen ) {
    return recv( m_sock, buf, bufLen, 0 );
}

