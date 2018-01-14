#ifndef _MYNETLIB_H_
#define _MYNETLIB_H_
#include <stdlib.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_CON_ADDR "127.0.0.1"
//#define DEFAULT_CON_ADDR "192.168.13.207"


void getFamilyByCode( char *buf, size_t bufSize, int code ) {
    switch( code ) {
        case 0: strncpy( buf, "AF_UNSPEC", bufSize ); break;
        case 2: strncpy( buf, "AF_INET", bufSize ); break;
        case 23: strncpy( buf, "AF_INET6", bufSize ); break;
        default: strncpy( buf, "ERROR", bufSize ); break;
    }
}
void getSockTypeByCode( char *buf, size_t bufSize, int code ) {
    switch( code ) {
        case 1: strncpy( buf, "SOCK_STREAM", bufSize ); break;
        case 2: strncpy( buf, "SOCK_DGRAM", bufSize ); break;
        case 3: strncpy( buf, "SOCK_RAW", bufSize ); break;
        case 4: strncpy( buf, "SOCK_RDM", bufSize ); break;
        default: strncpy( buf, "ERROR", bufSize ); break;
    }
}

void getProtocolByCode( char *buf, size_t bufSize, int code ) {
    switch( code ) {
        case 6: strncpy( buf, "IPPROTO_TCP", bufSize ); break;
        case 17: strncpy( buf, "IPPROTO_UDP", bufSize ); break;
        case 113: strncpy( buf, "IPPROTO_RM", bufSize ); break;
        default: strncpy( buf, "ERROR", bufSize ); break;
    }
}
void printAddrInfo( struct addrinfo * ptrAInf ) {
    
    for( struct addrinfo * tmp = ptrAInf; tmp != NULL; tmp = tmp->ai_next ) {

        char buf[512];
        getFamilyByCode( buf, 512, tmp->ai_family );
        printf( "    family: %s (%d)\n",    buf, tmp->ai_family );

        getSockTypeByCode( buf, 512, tmp->ai_socktype );
        printf( "  socktype: %s (%d)\n",    buf, tmp->ai_socktype );

        getProtocolByCode( buf, 512, tmp->ai_protocol );
        printf( "  protocol: %s (%d)\n",    buf , tmp->ai_protocol);

        unsigned short myPort = ntohs( ((sockaddr_in*)tmp->ai_addr)->sin_port );
        printf( "inet port2: %d\n",   myPort );

        printf( " canonname: %s\n",         tmp->ai_canonname );
        printf( " inet addr: 0x%x\n", ((sockaddr_in*)tmp->ai_addr)->sin_addr.s_addr );

    }
}

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

struct addrinfo * getClientAddrInfo( char * address ) {

    struct addrinfo hints, *ptrAInf;
    ptrAInf = NULL;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    int res = 0;

    /// Do we really need getaddrinfo for the server part?????
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    res = getaddrinfo( address, DEFAULT_PORT, &hints, &ptrAInf ); 
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


#endif
// vim: ff=dos fileencoding=utf8
