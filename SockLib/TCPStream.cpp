#include "tcpstream.h"
#if defined( _WIN32 ) || defined( _WIN64 )
#include <WS2tcpip.h>
#elif defined( _GNUG_ )
#include <arpa/inet.h>
#endif

TCPStream::TCPStream( SOCKET sck, struct sockaddr_in * addr ) 
    : m_sock( sck )
{
    char ipAddr[50] = {0};
    inet_ntop( PF_INET, (struct in_addr*)&(addr->sin_addr.s_addr), ipAddr, sizeof(ipAddr)-1 );
    m_addr = ipAddr;
    m_port = ntohs( addr->sin_port );
}

TCPStream::~TCPStream( ) {
#if defined( _WIN32 ) || defined( _WIN64 )
    closesocket( m_sock );
#elif defined( _GNUG_ )
	close( m_sock );
#endif
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

