#include "TCPAcceptor.h"
#include "TCPStream.h"

#if defined( _WIN32 ) || defined( _WIN64 )
#include <WS2tcpip.h>
#elif defined( _GNUG_ )
#endif

TCPAcceptor::TCPAcceptor( int port, char const * address )
#if defined( _WIN32 ) || defined( _WIN64 )
	: m_listenSocket( INVALID_SOCKET)
#elif defined( _GNUG_ )
	: m_listenSocket( -1 )
#endif
	, m_port(port)
	, m_address( address )
	, m_isListening( false )
{

}

TCPAcceptor::~TCPAcceptor() {
	if (m_listenSocket > 0) {
#if defined( _WIN32 ) || defined( _WIN64 )
		closesocket( m_listenSocket );
#elif defined( _GNUG_ )
		close( m_listenSocket );
#endif
	}
}

int TCPAcceptor::start() {
	if (m_isListening) {
		return 0;
	}

	int const bufLen = 128;
	char buf[bufLen] = {0};
	snprintf( buf, bufLen, "%d", m_port );

	struct addrinfo hints, *pAiRes, *pTmp;
	memset( &hints, 0, sizeof( hints ) );
	pAiRes = pTmp = nullptr;


	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int res = ::getaddrinfo( NULL, buf, &hints, &pAiRes );
	if (res == SOCKET_ERROR) {
		// TODO
		/*
		throw std::exception( "getaddrinfo" );
		*/
	}

	pTmp = pAiRes;
	while (pTmp != nullptr) {
		if( pTmp->ai_family == AF_INET ) break;
		pTmp = pTmp->ai_next;
	}
	if (pTmp == nullptr) {
		return -1;
	}

	m_listenSocket = ::socket( pTmp->ai_family, pTmp->ai_socktype, pTmp->ai_protocol );
	if (m_listenSocket == INVALID_SOCKET) {
		// TODO
		/*
		throw std::exception( "TCPAcceptor::start in socket" );
		*/
	}

	if (m_address.size() > 0) {
		inet_pton( PF_INET, m_address.c_str(), &((struct sockaddr_in *)pTmp)->sin_addr );
	}
	else {
		((struct sockaddr_in *)pTmp)->sin_addr.s_addr = INADDR_ANY;
	}

	int optval = 1;
	res = setsockopt( m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof optval );
	if (res == SOCKET_ERROR) {
		// TODO
		/*
		throw std::exception( "setsockopt" );
		*/
	}

	res = ::bind( m_listenSocket, pTmp->ai_addr, (int)pTmp->ai_addrlen );
	if( res == SOCKET_ERROR ) {
		// TODO
		/*
		throw std::exception( "bind" );
		*/
	}

	res = ::listen( m_listenSocket, 5 );
	if( res == SOCKET_ERROR ) {
		// TODO
		/*
		throw std::exception( "listen" );
		*/

	}
	m_isListening = true;

	return res;
}

TCPStream * TCPAcceptor::accept() {
	if (!m_isListening) {
		return nullptr;
	}

	struct sockaddr_in sockAddr;
	memset( &sockAddr, 0, sizeof( struct sockaddr_in ) );
	int addrLen = 0;

#if defined( _WIN32 ) || defined( _WIN64 )
	SOCKET clSock = INVALID_SOCKET;
#elif defined( _GNUG_ )
	int clSock = -1;
#endif

	clSock = ::accept( m_listenSocket, (struct sockaddr*)&sockAddr, &addrLen );

#if defined( _WIN32 ) || defined( _WIN64 )
	if( clSock == INVALID_SOCKET ) {
		//error
	}
#elif defined( _GNUG_ )
	if (clSock < 0) {
		perror( "accept" );
		return nullptr;
	}
#endif

	return new TCPStream( clSock, &sockAddr );
}
