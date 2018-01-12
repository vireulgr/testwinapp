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

struct addrinfo * getServerAddrInfo( ) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET; // TODO make this IPv4/IPv6 independent
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

long long simpleSend( SOCKET sock, char const * sendBuf, size_t const bufLen ) {
    size_t offset = 0;
    int res = 0;
    do {
        res = send( sock, sendBuf + offset, bufLen, 0 );
        if( res > 0 ) {
            offset += res;
            printf( "sent  % 12d bytes\ntotal % 12zd bytes", res, offset );
        }
        else if( res == 0 ) {
            puts( "closing connection" );
        }
        else if( res == SOCKET_ERROR ) {
            printf( "send failed! %d\n", WSAGetLastError() );
            closesocket( sock );
            return res;
        }
    } while( res > 0 );
    return offset;
}

long long simpleReceive( SOCKET sock, char * recvBuf, size_t const bufLen ) {
    int res = 0;
    size_t offset = 0;
    do {
        res = recv( sock, recvBuf + offset, bufLen, 0 );
        if( res > 0 ) {
            offset += res;
            printf( "received % 12d bytes\ntotal    % 12zd bytes", res, offset );
        }
        else if( res == 0 ) {
            puts( "closing connection" );
        }
        else if( res == SOCKET_ERROR ) {
            printf( "recv failed! %d\n", WSAGetLastError() );
            closesocket( sock );
            return res;
        }
    } while( res > 0 );

    recvBuf[offset + 1] = '\0';
    return offset;
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
    SERVER PART
*/
    struct addrinfo * tmp = NULL;
    struct addrinfo * ptrAInf = getServerAddrInfo();

    // walk through getaddrinfo results
    printAddrInfo( ptrAInf );

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

    // RECEIVE
    size_t const bufLen = 2048;
    //size_t offset = 0;
    char * recvBuf;
    recvBuf = new char[ bufLen ];
    //do {
    //    res = recv( clientSocket, recvBuf + offset, bufLen, 0 );
    //    if( res > 0 ) {
    //        offset += res;
    //        printf( "received % 12d bytes\ntotal    % 12zd bytes", res, offset );
    //    }
    //    else if( res == 0 ) {
    //        puts( "closing connection" );
    //    }
    //    else if( res < 0 ) {
    //        printf( "recv failed! %d\n", WSAGetLastError() );
    //        closesocket( clientSocket );
    //        return 1;
    //    }
    //} while( res > 0 );

    res = simpleReceive( clientSocket, recvBuf, bufLen );
    //recvBuf[offset + 1] = '\0';
    if( res != SOCKET_ERROR )  {
        printf( "received: %s\n", recvBuf );
    }
    else {
        printf( "error receive \n" );
    }
    delete[] recvBuf;

    // SEND
    char *sendBuf = "String from SERVER to send to CLIENT";

    // res = send( clientSocket, sendBuf, strlen( sendBuf ), 0 );
    res = simpleSend( clientSocket, sendBuf, strlen( sendBuf ) );
    if( res == SOCKET_ERROR ) {
        printf( "send failed! %ld", WSAGetLastError() );
        WSACleanup();
        //closesocket( clientSocket ); socket already closed by simpleSend
        return 1;
    }

    printf( "Bytes sent: %d (%zd)\n", res, strlen( sendBuf ) );

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

// vim: ff=dos
