#include "TCPConnector.h"
#include "TCPStream.h"
//#include <tchar.h>
//#include <string>
#include <stdio.h>
#if defined( _WIN32 ) || defined( _WIN64 )
#include <WS2tcpip.h>
#elif defined( _GNUG_ )
#endif

TCPStream * TCPConnector::connect(int port, char const * addr) {
	struct sockaddr_in hostAddr;
	memset( &hostAddr, 0, sizeof( hostAddr ) );
	resolveHost( addr, &hostAddr );
#if defined( _WIN32 ) || defined( _WIN64 )
	SOCKET s = INVALID_SOCKET;
#elif defined( _GNUG_ )
	int s = -1;
#endif
	s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
#if defined( _WIN32 ) || defined( _WIN64 )
	if (s == INVALID_SOCKET) {
		char buf[256] = {0};
		sprintf_s(  buf, "socket %d", WSAGetLastError() );
		throw std::exception(buf);
	}
#elif defined( _GNUG_ )
	if (s < 0) {
		char buf[256] = {0};
		sprintf(  buf, "socket %d", errno );
		throw std::exception(buf);
	}
#endif
	
	int res = ::connect( s, (struct sockaddr *)&hostAddr, sizeof( struct sockaddr_in ) );
	if( res != 0 ) {
		char buf[256] = {0};
		sprintf_s(  buf, "connect %d", WSAGetLastError() );
		throw std::exception(buf);
	}
	return new TCPStream( s, &hostAddr );
}

int TCPConnector::resolveHost(char const * host, struct sockaddr_in * in_addr) {
	struct addrinfo * pAiRes = NULL, hint, *pTmp = NULL;
	memset( &hint, 0, sizeof( hint ) );
	getaddrinfo( host, NULL, &hint, &pAiRes );
	// TODO
	return 1;
}
