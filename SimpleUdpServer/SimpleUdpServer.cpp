#include "../MyNetLib/MyNetLib.h"
#include <WinSock2.h>
#include <cstdio>
#include <WS2tcpip.h>
#include <memory>

namespace NL = MyNetLib;

unsigned short const g_port = 514;

int main()
{
    NL::Autocleanup a;
    int ret = a.init();
    if (ret == 0) {
        ::printf("error in WSAStartup, %d", ::WSAGetLastError());
        return 1;
    }

    struct addrinfo* tmp = nullptr;
    struct addrinfo* pGaiRes = NL::getUdpServerAddrInfo();
    for (tmp = pGaiRes; tmp != nullptr; tmp = tmp->ai_next) {
        if (tmp->ai_family == AF_INET) break;
    }

    if (tmp == nullptr) {
        ::printf("can't find suitable addrinfo\n");
        freeaddrinfo(pGaiRes);
        return 1;
    }

    SOCKET listenSock = ::socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
    if (listenSock == INVALID_SOCKET) {
        ::printf("error in socket, %d", ::WSAGetLastError());
        ::freeaddrinfo(pGaiRes);
        return 1;
    }

    ret = ::bind(listenSock, tmp->ai_addr, tmp->ai_addrlen);

    if (ret != 0) {
        ::printf("error in bind, %d", ::WSAGetLastError());
        ::freeaddrinfo(pGaiRes);
        ::closesocket(listenSock);
        return 1;
    }

    long long bufLen = 2048;
    std::unique_ptr<char[]> recvBuf = std::make_unique<char[]>(bufLen);
    long long received = 0;
    struct sockaddr_in* from = nullptr;
    while (true) {
        received = NL::simpleUdpReceive(listenSock, recvBuf.get(), bufLen, &from);
        wchar_t inetAddrBuf[128] = { 0 };
        ::InetNtopW(AF_INET, &(from->sin_addr), inetAddrBuf, 128);
        ::wprintf(L"received from %s\n", inetAddrBuf);
        ::printf("%s\n", recvBuf.get());
    }

    ::closesocket(listenSock);
    ::freeaddrinfo(pGaiRes);

    return 0;
}
