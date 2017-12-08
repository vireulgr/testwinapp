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

#define DEFAULT_PORT "27015"


int main( int argc, char * argv[] ) {

    WSADATA wsadata;
    int res;
    res = WSAStartup( MAKEWORD( 2, 2), &wsadata );
    if( res != 0 ) {
        printf( "WSAStartup failed! res = %d\n", res );
        exit( 1 );
    }

/*
    SERVER PART
*/
    struct addrinfo hints, *ptrAInf, *tmp;
    ptrAInf = NULL;
    tmp = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;
    hints.ai_flags      = AI_PASSIVE;

    res = getaddrinfo( NULL, DEFAULT_PORT, &hints, &ptrAInf ); 
    if( res != 0 ) {
        printf( "getaddrinfo failed! res = %d\n", res );       
        WSACleanup();
        return 1;
    }

    // walk through getaddrinfo results
    void printAddrInfo( ptrAInf );

    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket( ptrAInf->ai_family, ptrAInf->ai_socktype, ptrAInf->ai_protocol );
    if( listenSocket == INVALID_SOCKET ) {
        printf( "socket failed! %ld\n", WSAGetLastError() );
        freeaddrinfo( ptrAInf );
        WSACleanup();
        return 1;
    }

    res = bind( listenSocket, ptrAInf->ai_addr, (int)ptrAInf->ai_addrlen );
    if( res == SOCKET_ERROR ) {
        printf( "bind failed! %d\n", WSAGetLastError() );
        freeaddrinfo( ptrAInf );
        closesocket( listenSocket );
        WSACleanup();
        return 1;
    }

    freeaddrinfo( ptrAInf );

    if( listen( listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "listen failed! %d\n", WSAGetLastError() );
        closesocket( listenSocket );
        WSACleanup();
        return 1;
    }

    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
 
    clientSocket = accept( listenSocket, NULL, NULL );
    if( clientSocket == INVALID_SOCKET ) {
        printf( "accept failed! %d\n", WSAGetLastError() );
        closesocket( listenSocket );
        WSACleanup();
        return 1;
    }

    // TODO Receiving and Sending data
    size_t const bufLen = 2048;
    size_t offset = 0;
    char * recvBuf;
    recvBuf = new char[ bufLen ];
    do {
        res = recv( clientSocket, recvBuf + offset, bufLen, 0 );
        if( res > 0 ) {
            offset += res;
            
        }
        else if( res == 0 ) {
            puts( "closing connection" );
        }
        else if( res < 0 ) {
            printf( "recv failed! %d\n", WSAGetLastError() );
            closesocket( clientSocket );
            return 1;
        }
    } while( res > 0 );

    delete[] recvBuf;

    res = shutdown( clientSocket, SD_SEND );
    if( res == SOCKET_ERROR ) {
        printf( "shutdown failed! %d\n", WSAGetLastError() );
        closesocket( clientSocket );
        WSACleanup();
        return 1;
    }

    closesocket( listenSocket );
    WSACleanup();

/*
    CLIENT PART
*/

    return 0;
}