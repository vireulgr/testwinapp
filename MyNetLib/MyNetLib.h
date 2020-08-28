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

	struct addrinfo * getTcpServerAddrInfo();
	struct addrinfo * getTcpClientAddrInfo(char const * address);

	struct addrinfo * getUdpServerAddrInfo();
	struct addrinfo * getUdpClientAddrInfo(char const * address);

	long long simpleTcpSend(SOCKET const sock, char const * sendBuf, long long const bufLen, long long & sent);
	long long simpleTcpReceive(SOCKET const sock, char * recvBuf, long long const bufLen, long long & received);

	long long simpleUdpReceive(SOCKET const sock, char * recvBuf, long long const bufLen, struct sockaddr_in ** from);
}
