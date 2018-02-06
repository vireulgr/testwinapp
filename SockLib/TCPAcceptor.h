#ifndef _TCPACCEPTOR_H_
#define _TCPACCEPTOR_H_
#if defined( _WIN32 ) || defined( _WIN64 )
#include <WinSock2.h>
#endif
#include "TCPStream.h"

class TCPStream;

class TCPAcceptor {
#if defined( _WIN32 ) || defined( _WIN64 )
	SOCKET m_listenSocket;
#elif defined( _GNUG_ )
	int m_listenSocket;
#endif
	int m_port;
	std::string m_address;
	bool m_isListening;
public:
	TCPAcceptor( int port, char const * address = "" );
	~TCPAcceptor();
	int start();
	TCPStream * accept();
private:
	TCPAcceptor() {}
};

#endif
