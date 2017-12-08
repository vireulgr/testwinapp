#ifndef _MYNETLIB_H_
#define _MYNETLIB_H_
#include <stdlib.h>

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
    
    for( struct addinfo * tmp = ptrAInf;tmp != NULL; tmp = tmp->ai_next ) {
    //while( tmp != NULL ) {
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

        //tmp = tmp->ai_next;
    }
}

#endif
