#include <stdio.h>
/*
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
*/
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include "mynetlib.h"
#include <thread>
#include <chrono>
//#include "mylib.h"
// script for PowerShell for( $i=0; $i -le 5; ++$i ) { Start-Process D:\prog\src\cpp\winsock\x64\debug\client.exe }

#pragma comment( lib, "Ws2_32.lib" )

int main( int argc, char * argv[] ) {

    WSADATA wsadata;
    int res;
    res = WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
    if( res != 0 ) {
        printf( "WSAStartup failed! res = %d\n", res );
        return 1;
    }

    struct addrinfo *ptrAInf, *tmp = NULL;
    ptrAInf = getClientAddrInfo( DEFAULT_CON_ADDR );

    // walk through getaddrinfo results
    printAddrInfo( ptrAInf );

    tmp = ptrAInf;
	while( tmp != nullptr ) {
		if( tmp->ai_family == AF_INET )
			break;
		tmp = tmp->ai_next;
	}
    if( tmp == nullptr ) {
        puts( "no ipv4 addrinfo" );
        freeaddrinfo( ptrAInf );
        WSACleanup();
        return 1;
    }

    char strBuf[1024] = {0};
    InetNtop( tmp->ai_family, &(((sockaddr_in*)tmp->ai_addr)->sin_addr), strBuf, 1024 );

    printf( "address: %s\n", strBuf );

    SOCKET connSocket = INVALID_SOCKET;
    connSocket = socket( tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol );
    if( connSocket == INVALID_SOCKET ) {
        printf( "socket failed! %ld\n", WSAGetLastError() );
        freeaddrinfo( ptrAInf );
        WSACleanup();
        return 1;
    }

    res = connect( connSocket, tmp->ai_addr, (int)tmp->ai_addrlen );
    if( res == SOCKET_ERROR ) {
        printf( "connect failed! %ld", WSAGetLastError() );
        closesocket( connSocket );
		freeaddrinfo( ptrAInf );
		WSACleanup();
		std::this_thread::sleep_for( std::chrono::seconds(2) );
		return -1;
    }

    freeaddrinfo( ptrAInf );

    // Should try next address in ptrAInf list
    if( connSocket == INVALID_SOCKET ) {
        printf( "Unable to connect to server!" );
        WSACleanup();
        return 1;
    }

    DWORD myPid = GetCurrentProcessId();
    printf( "PID>>  %d  <<\n", myPid );

    // SEND
    char *sendFormat = "PID>>  %zd  <<\tBegin\n"
        "1The very long String from client to send to server\nIts lengh is way"
        "longer than typical buffer length (I hope). Yo! Repeat!\n"
        "2The very long String from client to send to server\nIts lengh is way"
        "longer than typical buffer length (I hope). Yo! Repeat!\n"
        "3The very long String from client to send to server\nIts lengh is way"
        "longer than typical buffer length (I hope). Yo! Repeat!\n"
        "4The very long String from client to send to server\nIts lengh is way"
        "longer than typical buffer length (I hope). Yo! Repeat!\n"
        "End\n"
        ;
    char *sendBuf = new char[1024];
    memset( sendBuf, 0, 1024 );
    snprintf( sendBuf, 1024 * sizeof( char ), sendFormat, myPid );

    for( int cn=0; cn<4; ++cn ) {
        res = send( connSocket, sendBuf, (int)strlen( sendBuf ), 0 );
        if( res == SOCKET_ERROR ) {
            printf( "send failed! %ld", WSAGetLastError() );
            WSACleanup();
            closesocket( connSocket );
            return 1;
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }


    printf( "Bytes sent: %d (%zd)\n", res, strlen( sendBuf ) );

    res = shutdown( connSocket, SD_SEND );
    if( res == SOCKET_ERROR ) {
        printf( "shutdown failed! %d\n", WSAGetLastError() );
        closesocket( connSocket );
        WSACleanup();
        return 1;
    }
    // END SEND

    // RECEIVE
    int const bufLen = 2048;
    char * recvBuf;
    int offset = 0;
    recvBuf = new char[ bufLen ];
    do {
        res = recv( connSocket, recvBuf + offset, bufLen - offset, 0 );
        if( res > 0 ) {
            offset += res;
            printf( "received % 12d bytes\ntotal    % 12d bytes", res, offset );
        }
        else if( res == 0 ) {
            puts( "closing connection" );
        }
        else if( res < 0 ) {
            printf( "recv failed! %d\n", WSAGetLastError() );
            closesocket( connSocket );
            WSACleanup();
            return 1;
        }
    } while( res > 0 );

    recvBuf[offset + 1] = '\0';
    printf( "received: %s\n", recvBuf );
    delete[] recvBuf;

    // END RECEIVE

    closesocket( connSocket );
    WSACleanup();

    return 0;
}

// vim: ff=dos fileencoding=utf8 expandtab
