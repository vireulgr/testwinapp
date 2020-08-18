#pragma once

#include <WinSock2.h>

#define DEFAULT_PORT_INT 27015
#define DEFAULT_PORT_STR "27015"
#define DEFAULT_CON_ADDR "127.0.0.1"
//#define DEFAULT_CON_ADDR "192.168.13.207"

namespace MyNetLib {
	class Autocleanup {
	public:
		Autocleanup();
		Autocleanup(Autocleanup &) = delete;
		Autocleanup& operator=(Autocleanup const&) = delete;
		~Autocleanup();
		int init();
		void reset();
	private:
		bool m_isInited;
		WSADATA m_wsaData;
	};
	//int init();
	//void deinit();
	char const * wsaErrorToString(int err);
	char const * getFamilyByCode( int code );
	char const * getSockTypeByCode( int code );
	char const * getProtocolByCode( int code );
	void printAddrInfo( struct addrinfo * ptrAInf );
	struct addrinfo * getServerAddrInfo( );
	struct addrinfo * getClientAddrInfo( char * address );
	int simpleSend( SOCKET const sock, char const * sendBuf, int const bufLen, long long & sent );
	int simpleReceive( SOCKET const sock, char * recvBuf, int const bufLen, long long & received );
}
