#include "../MyNetLib/MyNetLib.h"
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
#include <cstdio>
#include <memory>
#include <mutex>

namespace NL = MyNetLib;

//unsigned short const g_port = 27182;

std::mutex g_mtx;
bool g_doWork = true;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        g_mtx.lock();
        g_doWork = false;
        g_mtx.unlock();
    }
    return TRUE;
}

int main()
{
#ifdef _WIN32
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        ::printf("Error! cannot set console handler");
        return 1;
    };
#endif

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

        g_mtx.lock();
        bool doWorkValue = g_doWork;
        g_mtx.unlock();

        if (!doWorkValue) break;
#ifdef _WIN32
        Sleep(500);
#endif
    }

    ::closesocket(listenSock);
    ::freeaddrinfo(pGaiRes);

    return 0;
}
