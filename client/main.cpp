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
#include "mylib.h"

#pragma comment( lib, "Ws2_32.lib" )

struct addrinfo * getClientAddrInfo( ) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
    hints.ai_flags      = AI_PASSIVE;

    res = getaddrinfo( NULL, DEFAULT_PORT, &hints, &ptrAInf ); 
    if( res != 0 ) {
        printf( "getaddrinfo failed! res = %d\n", res );       
        WSACleanup();
        return NULL;
    }

    return ptrAInf;
}

int main( int argc, char * argv[] ) {

    WSADATA wsadata;
    int res;
    res = WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
    if( res != 0 ) {
        printf( "WSAStartup failed! res = %d\n", res );
        return 1;
    }
/*
    CLIENT PART
*/
    // struct addrinfo hints, *ptrAInf, *tmp;
    // ptrAInf = NULL;
    // tmp = NULL;
    // memset( &hints, 0, sizeof( struct addrinfo ) );

    // hints.ai_family     = AF_UNSPEC;
    // hints.ai_socktype   = SOCK_STREAM;
    // hints.ai_protocol   = IPPROTO_TCP;

    // res = getaddrinfo( DEFAULT_CON_ADDR, DEFAULT_PORT, &hints, &ptrAInf ); 
    // if( res != 0 ) {
    //     printf( "getaddrinfo failed! res = %d\n", res );       
    //     WSACleanup();
    //     return 1;
    // }
    struct addrinfo *ptrAInf, *tmp = NULL;
    ptrAInf = getClientAddrInfo();

    // walk through getaddrinfo results
    printAddrInfo( ptrAInf );

    tmp = ptrAInf;
    if( tmp != nullptr && tmp->ai_family != AF_INET ) { tmp = tmp->ai_next; }
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
    }

    freeaddrinfo( ptrAInf );

    // Should try next address in ptrAInf list
    if( connSocket == INVALID_SOCKET ) {
        printf( "Unable to connect to server!" );
        WSACleanup();
        return 1;
    }


    // SEND
    char *sendBuf = "String from client to send to server";

    res = send( connSocket, sendBuf, strlen( sendBuf ), 0 );
    if( res == SOCKET_ERROR ) {
        printf( "send failed! %ld", WSAGetLastError() );
        WSACleanup();
        closesocket( connSocket );
        return 1;
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
    size_t const bufLen = 2048;
    char * recvBuf;
    size_t offset = 0;
    recvBuf = new char[ bufLen ];
    do {
        res = recv( connSocket, recvBuf + offset, bufLen - offset, 0 );
        if( res > 0 ) {
            offset += res;
            printf( "received % 12d bytes\ntotal    % 12zd bytes", res, offset );
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

// vim: ff=dos
