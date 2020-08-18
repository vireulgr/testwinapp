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

#include "..\MyNetLib\MyNetLib.h"
//#include "mylib.h"

#include <thread>
#include <chrono>

#pragma comment( lib, "Ws2_32.lib" )

class ClientsSocksArray {

    int m_numClients;
    __int64	m_maxFD;
    SOCKET* m_clientSocks;

public:
    class iterator {
        SOCKET m_emergency;
        SOCKET * m_ptr;
    public:
        iterator( SOCKET * ptr )
            : m_emergency( INVALID_SOCKET), m_ptr(ptr) {}
        void next() { ++m_ptr; }
        void prev() { --m_ptr; }
        bool operator!=( ClientsSocksArray::iterator const & other ) {
            return m_ptr != other.m_ptr;
        }
        bool operator==( ClientsSocksArray::iterator const & other ) {
            return m_ptr == other.m_ptr;
        }
        SOCKET & operator*() {
            if( m_ptr != nullptr ) {
                return *(m_ptr);
            }
            else {
                return m_emergency;
            }
        }
        SOCKET * operator->() {
            if( m_ptr != nullptr ) {
                return m_ptr;
            }
            else {
                return &m_emergency;
            }
        }
    };

    size_t const MAX_CLIENTS = 5;

    ClientsSocksArray()
        : m_numClients(0)
        , m_maxFD(0)
    {
        m_clientSocks = new SOCKET[MAX_CLIENTS];
        for( int i=0; i<MAX_CLIENTS; ++i ) {
            m_clientSocks[i] = INVALID_SOCKET;
        }
    }

    ~ClientsSocksArray() {
        if (m_clientSocks) {
            delete [] m_clientSocks;
        }
    }

    bool isFull() {
        return m_numClients == MAX_CLIENTS;
    }

    void addClient(SOCKET client) {
        m_clientSocks[m_numClients] = client;
        m_maxFD = m_maxFD < client ? client : m_maxFD;
        ++m_numClients;
    }

    __int64 getMaxFD() {
        return m_maxFD;
    }

    SOCKET getClient(int idx) {
        if( idx >= MAX_CLIENTS && idx < 0 ) {
            return INVALID_SOCKET;
        }
        return m_clientSocks[idx];
    }

    void removeClient( ClientsSocksArray::iterator it ) {
        if( it == this->end() ) {
            return;
        }
        --m_numClients;
        __int64 temp = *it;
        *it = m_clientSocks[m_numClients];
        m_clientSocks[m_numClients] = INVALID_SOCKET;
        if (temp == m_maxFD) {
            m_maxFD = countMaxFD();
        }
    }

    void removeClient( int idx ) {
        if( idx >= MAX_CLIENTS && idx < 0 ) {
            return;
        }
        --m_numClients;
        __int64 temp = m_clientSocks[idx];
        m_clientSocks[idx] = m_clientSocks[m_numClients];
        m_clientSocks[m_numClients] = INVALID_SOCKET;
        if (temp == m_maxFD) {
            m_maxFD = countMaxFD();
        }
    }

    ClientsSocksArray::iterator begin() {
        return ClientsSocksArray::iterator( m_clientSocks );
    }
    ClientsSocksArray::iterator end() {
        return ClientsSocksArray::iterator( m_clientSocks+m_numClients );
    }
private:
    __int64 countMaxFD() {
        __int64 res = 0;
        for( int i = 0; i < m_numClients; ++i ) {
            res = res < m_clientSocks[i] ? m_clientSocks[i] : res;
        }
        return res;
    }
};

void addClientsToFDSet( fd_set * set, ClientsSocksArray & clients ) {
    ClientsSocksArray::iterator it = clients.begin();
    for( ; it != clients.end(); it.next() ) {
        FD_SET( *it, set );
    }
}

#define INIT_WSA2( res ) do {							\
    WSADATA wsadata;									\
    res = WSAStartup( MAKEWORD( 2, 2 ), &wsadata );		\
    if( res != 0 ) {									\
        printf( "WSAStartup failed! res = %d\n", res );	\
        return 1;										\
    } } while( false )									\

int main() {

    //int res;
    //INIT_WSA2( res );
    MyNetLib::Autocleanup a;
    int res = a.init();
    if( res != 0 ) {
        printf( "WSAStartup failed! res = %d\n", res );
        return 1;
    }
    //WSADATA wsadata;
    //int res;
    //res = WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
    //if( res != 0 ) {
    //    printf( "WSAStartup failed! res = %d\n", res );
    //    return 1;
    //}
/*
    SERVER PART
*/
    struct addrinfo * tmp = NULL;
    struct addrinfo * ptrAInf = MyNetLib::getServerAddrInfo();

    // walk through getaddrinfo results
    MyNetLib::printAddrInfo( ptrAInf );

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

    /**
    */
    unsigned int maxSockFd = 0;
    size_t const MAX_CLIENTS = 5;
    struct fd_set inFds;
    ClientsSocksArray clientSocks;

    while( 1 ) {
        FD_ZERO( &inFds );
        // setup in_fds;
        if (!clientSocks.isFull()) {
            FD_SET( listenSocket, &inFds );
        }
        addClientsToFDSet( &inFds, clientSocks );

        struct timeval tv = { 0, 5000 };
        // ==> SELECT! <==
        res = select( maxSockFd, &inFds, NULL, NULL, &tv );
        if( res == 0 ) { // timeout
            //std::this_thread::sleep_for( std::chrono::milliseconds(500) );
            Sleep(500);
            continue;
        }
        else if( res < 0 ) { // error
            printf( "select failed %d\n", WSAGetLastError() );
            break;
        }
        else { // there are sockets in set
            if( FD_ISSET( listenSocket, &inFds ) ) {
                SOCKET temp = INVALID_SOCKET;
                temp = accept( listenSocket, NULL, NULL );
                if( temp == INVALID_SOCKET ) {
                    printf( "accept failed %d\n", WSAGetLastError() );
                    break;
                }
                if( !clientSocks.isFull() ) {
                    clientSocks.addClient( temp );
                }
            }
            ClientsSocksArray::iterator it = clientSocks.begin();
            for( ; it != clientSocks.end(); it.next() ) {
                if( *it == INVALID_SOCKET ) continue;
                if( !FD_ISSET( *it, &inFds ) ) continue;

                int const theLen = 8192;
                char theBuffer[theLen] = {0};
                long long readRet = 0;

                int recRes = MyNetLib::simpleReceive( *it, theBuffer, theLen, readRet );

                res = shutdown( *it, SD_BOTH );
                if( res == SOCKET_ERROR ) {
                    printf( "shutdown failed! %d\n", WSAGetLastError() );
                    closesocket( *it );
                    WSACleanup();
                    return 1;
                }
                res = closesocket( *it );
                if (res == SOCKET_ERROR) {
                    printf( "closesocket failed! %d\n", WSAGetLastError() );
                    WSACleanup();
                    return 1;
                }
                // remove client socket
                clientSocks.removeClient( it );
                it.prev(); // collection has been modified, so need to go back

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
/* vim: set ff=dos fileencoding=utf8 expandtab : */
