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
//#include "mylib.h"

#include <thread>
#include <chrono>

#pragma comment( lib, "Ws2_32.lib" )


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

    ///
    /**
    */
    unsigned int maxSockFd = 0;
    size_t const MAX_CLIENTS = 5;
    struct fd_set inFd;
    int numClients = 0;
    SOCKET clientSocks[MAX_CLIENTS];
    for( int i=0; i<MAX_CLIENTS; ++i ) {
        clientSocks[i] = INVALID_SOCKET;
    }

    while( 1 ) {
        FD_ZERO( &inFd );
        if( numClients < MAX_CLIENTS ) {
            FD_SET( listenSocket, &inFd );
        }
        maxSockFd = listenSocket;
        for( int i=0; i<MAX_CLIENTS; ++i ) {
            if( clientSocks[i] == INVALID_SOCKET ) continue;
            maxSockFd = clientSocks[i] > maxSockFd ? clientSocks[i] : maxSockFd;
            FD_SET( clientSocks[i], &inFd );
        }

        struct timeval tv = { 0, 5000 };
        res = select( maxSockFd, &inFd, NULL, NULL, &tv );
        if( res < 0 ) { // error
            printf( "select failed %d\n", WSAGetLastError() );
        }
        else if( res == 0 ) { // timeout
            std::this_thread::sleep_for( std::chrono::milliseconds(500) );
            continue;
        }
        else { // there are sockets in set
            if( FD_ISSET( listenSocket, &inFd ) ) {
                clientSocks[numClients] = accept( listenSocket, NULL, NULL );
                if( clientSocks[ numClients ] == INVALID_SOCKET ) {
                    printf( "accept failed %d\n", WSAGetLastError() );
                    break;
                }
                ++numClients;
            }
            for( int i=0; i<numClients; ++i ) {
                if( clientSocks[i] == INVALID_SOCKET ) continue;
                if( !FD_ISSET( clientSocks[i], &inFd ) ) continue;

                int const theLen = 8192;
                char theBuffer[theLen] = {0};
                long long readRet = 0;

                int recRes = simpleReceive( clientSocks[i], theBuffer, theLen, readRet );

                res = shutdown( clientSocks[i], SD_BOTH );
                if( res == SOCKET_ERROR ) {
                    printf( "shutdown failed! %d\n", WSAGetLastError() );
                    closesocket( clientSocks[i] );
                    WSACleanup();
                    return 1;
                }
                res = closesocket( clientSocks[i] );
                if (res == SOCKET_ERROR) {
                    printf( "closesocket failed! %d\n", WSAGetLastError() );
                    WSACleanup();
                    return 1;
                }
                // switch last active socket and closed socket
                --numClients;
                clientSocks[i] = clientSocks[numClients];
                clientSocks[numClients] = INVALID_SOCKET;

                if( readRet > 0 ) {
                    theBuffer[theLen-1]='\0';
                    //printf( "received:\n\t%s\n", theBuffer);
                    // TODO DEBUG
                    theBuffer[10]='\0';
                    theBuffer[11]='\0';
                    printf( "PID>>  %s  <<\n", theBuffer);
                }
            }
        }
    }

    res = shutdown( listenSocket, SD_SEND );
    if( res == SOCKET_ERROR ) {
        printf( "shutdown failed! %d\n", WSAGetLastError() );
        closesocket( listenSocket );
        WSACleanup();
        return 1;
    }

    closesocket( listenSocket );
    WSACleanup();

    return 0;
}

// vim: ff=dos fileencoding=utf8 expandtab
