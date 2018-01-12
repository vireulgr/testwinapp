#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main( int argc, char * argv[] ) {

    WSAData wsaData;
    int ret = 0;
    ret = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if( ret != 0 ) {
        return 1;
    }

    struct addrinfo hints, *ptrAInf = NULL, *pTmp = NULL;
    ZeroMemory( &hints, sizeof hints );
    hints.ai_family     = AF_INET;
    hints.ai_proto      = IPPROTO_TCP;
    hints.ai_socktype   = SOCK_STREAM;

    ret = getaddrinfo( "127.0.0.1", "20715", &hints, &ptrAInf );
    if( ret != 0 ) {
        WSACleanup();
        return 1;
    }
    
    for( pTmp = ptrAInf; pTmp != NULL; pTmp = pTmp->ai_next ) {
        if( pTmp->ai_family = AF_INET ) break;
    }
    if( pTmp == NULL ) {
        WSACleanup();
        freeaddrinfo( ptrAInf );
        return 1;
    }

    SOCKET connSocket = INVALID_SOCKET;
    connSocket = socket( ptrAInf->ai_family, ptrAInf->ai_socktype, ptrAInf->ai_proto );
    if( connSocket == INVALID_SOCKET ){
        WSACleanup();
        return 1;
    }

    while( true ) {
        fd_set fdsin;
        FD_ZERO( &fdsin );
        FD_SET( stdin, &fdsin );
        FD_SET( connSock, &fdsin );

        int maxFD = max( stdin, connSocket ) + 1;
        struct time_struct timeOut = { 0, 500000 };
        select( maxFD, &fdsin, NULL, NULL, *timeOut );

        if( FD_ISSET( stdin, fdsin ) ) {
            int const bufLen = 1024;
            int offset = 0;
            char theBuf[bufLen] = {0};
            while( true ) {
                // if buffer is ready transmit
                ret = read( STDIN_FILENO, theBuf + offset, bufLen - offset );
                if( ret == 0 ) { 
                    // connection closed
                    break;
                }
                else if( ret < 0 ) {
                    // error occured
                    printf( "error while read" );
                    break;
                }
                else {
                    offset += ret;
                }
            }
            // send to server
            {
                int const bufLen = 1024;
                int offset = 0;
                char theBuf[bufLen] = {0};
                while( true ) {
                    // if buffer is ready transmit
                    ret = write( connSocket, theBuff + offset, bufLen - offset );
                    if( ret == 0 ) { 
                        // connection closed
                        break;
                    }
                    else if( ret < 0 ) {
                        // error occured
                        printf( "error while read" );
                        break;
                    }
                    else {
                        offset += ret;
                    }
                }
            }
        }
        if( FD_ISSET( connSocket, fdsin ) ) {
            int const bufLen = 1024;
            int offset = 0;
            char theBuf[bufLen] = {0};
            while( true ) {
                // if buffer is ready transmit
                ret = read( connSocket, theBuf + offset, bufLen - offset );
                if( ret == 0 ) { 
                    // connection closed
                    break;
                }
                else if( ret < 0 ) {
                    // error occured
                    printf( "error while read" );
                    break;
                }
                else {
                    offset += ret;
                }
            }
            puts( theBuff );
        }
    }

    return 0;
}
