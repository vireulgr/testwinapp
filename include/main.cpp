//#define WIN32_LEAN_AND_MEAN   // required if winsock2.h is used along with windows.h
//#include <Windows.h>          // not needed if winsock2.h is used
#include <winsock2.h> // before iphlpapi.h
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment( lib, "ws2_32.lib" )

void printIpv6( unsigned char * in ) {
    printf( 
        "%x %x %x %x : %x %x %x %x : %x %x %x %x : %x %x %x %x"
        ,in[0] ,in[1] ,in[2]  ,in[3]  ,in[4]  ,in[5]  ,in[6]  ,in[7]
        ,in[8] ,in[9] ,in[10] ,in[11] ,in[12] ,in[13] ,in[14] ,in[15] 
    );
}

enum AiMembers {
    ai_family   = 0,
    ai_socktype = 1,
    ai_flags    = 2,
    ai_protocol = 3
};

#define CASE_STR_REPR( x ) case (x): return (#x);

char * getAiMemberString( int val, AiMembers type ) {
    static char buffer[128];
    switch( type ) {
    case ai_family:
        switch( val ) {
        CASE_STR_REPR( AF_UNSPEC )
        CASE_STR_REPR( AF_INET   )
        CASE_STR_REPR( AF_INET6  )
        default: 
        snprintf( buffer, 128, "Family unknown (%d)", val );
        return buffer;
        }
    break;
    case ai_socktype:
        switch( val ) {
        CASE_STR_REPR( SOCK_STREAM )
        CASE_STR_REPR( SOCK_DGRAM )
        default: 
        snprintf( buffer, 128, "Socktype unknown (%d)", val );
        return buffer;
        }
    break;
    case ai_flags:
        snprintf( buffer, 128, "%d", val );
        return buffer;
    break;
    case ai_protocol:
        switch( val ) {
        CASE_STR_REPR( IPPROTO_TCP )
        CASE_STR_REPR( IPPROTO_UDP )
        default: 
        snprintf( buffer, 128, "Proto unknown (%d)", val );
        return buffer;
        }
    break;
    default: 
    snprintf( buffer, 128, "Type unknown (%d); val (%d)", type, val );
    return buffer;
    }
    return buffer;
}

#undef CASE_STR_REPR

int main (void) {

    WSADATA wd;
    WSAStartup( MAKEWORD(2,0), &wd );

    struct addrinfo *resList;
    resList = nullptr;

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int res = getaddrinfo( "yandex.ru", nullptr, &hints, &resList );
    if( res != ERROR_SUCCESS ) {
        printf( "Error in getaddrinfo\n" );
        WSACleanup();
    }

    char ** temp;
    temp = new char *[4];
    for( int i = 0; i < 4; ++i ) {
        temp[i] = new char [256];
    }


        printf( "Before loop\n" );
    struct addrinfo *tmp = resList;
    while( tmp != nullptr ) {
        strncpy( temp[0], getAiMemberString( tmp->ai_family  , ai_family   ), 32);
        //printf( "%s\n", temp[0] );
        strncpy( temp[1], getAiMemberString( tmp->ai_socktype, ai_socktype ), 32);
        //printf( "%s\n", temp[1] );
        strncpy( temp[2], getAiMemberString( tmp->ai_flags   , ai_flags    ), 32);
        //printf( "%s\n", temp[2] );
        strncpy( temp[3], getAiMemberString( tmp->ai_protocol, ai_protocol ), 32);
        //printf( "%s\n", temp[3] );

        printf(
            "tmp->ai_family     0x%X (%s)\n"
            "tmp->ai_socktype   0x%X (%s)\n"
            "tmp->ai_flags      0x%X (%s)\n"
            "tmp->ai_protocol   0x%X (%s)\n"
            "tmp->ai_addrlen    0x%X\n"
            , tmp->ai_family   , temp[0]
            , tmp->ai_socktype , temp[1]
            , tmp->ai_flags    , temp[2]
            , tmp->ai_protocol , temp[3]
            , tmp->ai_addrlen
        );

        switch( tmp->ai_family ) {
            case AF_INET:
            printf( "IPv4 address: %s\n",
                inet_ntoa( ((sockaddr_in*)(tmp->ai_addr))->sin_addr /*.S_un.S_addr*/ )  
            );
            break;

            case AF_UNSPEC:
            printf( "Unspecified" );
            break;

            case AF_INET6:
            printf( "IPv6 address: " );
            printIpv6( ((sockaddr_in6*)(tmp->ai_addr))->sin6_addr.u.Byte );
            printf( "\n" );
            break;

            default: break;
        }

        tmp = tmp->ai_next;
    }


    for( int i = 0; i < 4; ++i ) {
        delete [] temp[i];
    }
    delete [] temp;

    freeaddrinfo( resList );
    WSACleanup();

    return 0;
}
