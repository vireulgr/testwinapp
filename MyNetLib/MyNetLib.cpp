﻿#include "pch.h"
#include "framework.h"

//#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h>

#include "MyNetLib.h"

#pragma comment( lib, "Ws2_32.lib" )

char const * MyNetLib::wsaErrorToString(int err) {
	char const* a = nullptr;
	switch (err) {
	case WSASYSNOTREADY: a = "The underlying network subsystem is not ready for network communication."; break;
	case WSAVERNOTSUPPORTED: a = "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."; break;
	case WSAEINPROGRESS: a = "A blocking Windows Sockets 1.1 operation is in progress."; break;
	case WSAEPROCLIM: a = "A limit on the number of tasks supported by the Windows Sockets implementation has been reached."; break;
	case WSAEFAULT: a = "The lpWSAData parameter is not a valid pointer."; break;
	default: a = "Unknown"; break;
	}
	return a;
}


MyNetLib::Autocleanup::Autocleanup() : m_isInited(false), m_wsaData() {}

MyNetLib::Autocleanup::~Autocleanup() {

    if (m_isInited) {
        ::WSACleanup();
    }
}
int MyNetLib::Autocleanup::init() {
    return ::WSAStartup(MAKEWORD(2, 2), &m_wsaData);
}

void MyNetLib::Autocleanup::reset() {
    m_isInited = false;
}


char const * MyNetLib::getFamilyByCode( int code ) {
    switch( code ) {
        case 0: return "AF_UNSPEC";
        case 2: return "AF_INET";
        case 23: return "AF_INET6";
        default: return "ERROR";
    }
}
char const * MyNetLib::getSockTypeByCode( int code ) {
    switch( code ) {
        case 1:  return "SOCK_STREAM";
        case 2:  return "SOCK_DGRAM";
        case 3:  return "SOCK_RAW";
        case 4:  return "SOCK_RDM";
        default: return "ERROR";
    }
}

char const * MyNetLib::getProtocolByCode( int code ) {
    switch( code ) {
        case 6:   return "IPPROTO_TCP";
        case 17:  return "IPPROTO_UDP";
        case 113: return "IPPROTO_RM";
        default:  return "ERROR";
    }
}

void MyNetLib::printAddrInfo( struct addrinfo * ptrAInf ) {
    
    for(struct addrinfo * tmp = ptrAInf; tmp != NULL; tmp = tmp->ai_next) {

        char const * str = getFamilyByCode(tmp->ai_family);
        printf("   family: %s (%d)\n", str, tmp->ai_family);

        str = getSockTypeByCode( tmp->ai_socktype );
        printf(" socktype: %s (%d)\n", str, tmp->ai_socktype);

        str = getProtocolByCode(tmp->ai_protocol);
        printf(" protocol: %s (%d)\n", str, tmp->ai_protocol);

        unsigned short port = ntohs(((sockaddr_in*)tmp->ai_addr)->sin_port);
        printf("inet port: %d\n", port );

        printf("canonname: %s\n", tmp->ai_canonname);
        printf("inet addr: 0x%x\n", ((sockaddr_in*)tmp->ai_addr)->sin_addr.s_addr);
    }
}

struct addrinfo * MyNetLib::getTcpServerAddrInfo( ) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET; // TODO make this IPv4/IPv6 independent
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
    hints.ai_flags      = AI_PASSIVE;

    res = getaddrinfo( NULL, DEFAULT_PORT_STR, &hints, &ptrAInf );
    if( res != 0 ) {
        printf( "getaddrinfo failed! res = %d\n", res );
        //WSACleanup(); // see Autocleanup
        return NULL;
    }

    return ptrAInf;
}

struct addrinfo * MyNetLib::getTcpClientAddrInfo(char const * address) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    res = getaddrinfo( address, DEFAULT_PORT_STR, &hints, &ptrAInf );
    if( res != 0 ) {
        printf( "getaddrinfo failed! res = %d\n", res );
        //WSACleanup(); // see Autocleanup
        return NULL;
    }

    return ptrAInf;
}

struct addrinfo * MyNetLib::getUdpServerAddrInfo( ) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET; // TODO make this IPv4/IPv6 independent
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_protocol   = IPPROTO_UDP;
    hints.ai_flags      = AI_PASSIVE;

    res = getaddrinfo(NULL, DEFAULT_PORT_STR, &hints, &ptrAInf);
    if(res != 0) {
        printf("getaddrinfo failed! res = %d\n", res);
        //WSACleanup(); // see Autocleanup
        return NULL;
    }

    return ptrAInf;
}

struct addrinfo * MyNetLib::getUdpClientAddrInfo(char const * address) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    int res = 0;

    // Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_protocol   = IPPROTO_UDP;

    res = getaddrinfo(address, DEFAULT_PORT_STR, &hints, &ptrAInf);
    if(res != 0) {
        printf("getaddrinfo failed! res = %d\n", res);
        //WSACleanup(); // see Autocleanup
        return NULL;
    }

    return ptrAInf;
}

long long MyNetLib::simpleTcpSend(SOCKET const sock, char const * sendBuf, long long const bufLen, long long & sent) {
    int offset = 0;
    int res = 0;
    do {
        res = send( sock, sendBuf + offset, bufLen, 0 );
        if( res > 0 ) {
            offset += res;
            printf( "sent  % 12d bytes\ntotal % 12d bytes", res, offset );
        }
        else if( res == 0 ) {
            puts( "Can't send" );
            sent = offset;
            return 0;
        }
        else if( res == SOCKET_ERROR ) {
            int err = WSAGetLastError();
            printf( "send failed! %d\n", err );
            //closesocket( sock );
            sent = offset;
            return err;
        }
    } while( res > 0 );

    sent = offset;
    return offset;
}

long long MyNetLib::simpleTcpReceive(SOCKET const sock, char * recvBuf, long long const bufLen, long long & received) {
    long long res = 0;
    long long offset = 0;
    do {
        offset += res;
        //printf( "received % 12d bytes\n   total % 12d bytes\n", res, offset );
        res = recv( sock, recvBuf + offset, bufLen - offset, 0 );
    } while( res > 0 && bufLen > offset );

   if( res == 0 ) {
       printf("Total received % 12lld bytes\n", offset);
   }
   else if( res == SOCKET_ERROR ) {
       int err = WSAGetLastError();
       printf( "recv failed! %d\n", err );
   }

    recvBuf[offset + 1] = '\0';
    received = offset;
    return res;
}

long long MyNetLib::simpleUdpReceive(SOCKET const sock, char * recvBuf, long long const bufLen, struct sockaddr_in & from) {
    long long res = 0;

    //printf( "received % 12d bytes\n   total % 12d bytes\n", res, offset );
    struct sockaddr_in fromStruct;
    int fromStructLen = sizeof(struct sockaddr_in);
    ::memset(&fromStruct, 0, fromStructLen);
    res = ::recvfrom(sock, recvBuf, bufLen, 0, (struct sockaddr*)&fromStruct, &fromStructLen);

    if (res == 0) {
        return res;
    }
    else if (res == SOCKET_ERROR) {
        int err = ::WSAGetLastError();
        ::printf("recvfrom failed! %d\n", err);
        return -1;
    }

    if (fromStructLen == sizeof(struct sockaddr_in)) {
        from = *((struct sockaddr_in*)&fromStruct);
    }

    recvBuf[res + 1] = '\0';

    return res;
}

// vim: ff=dos fileencoding=utf8 expandtab
