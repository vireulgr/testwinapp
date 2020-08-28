#include "../MyNetLib/MyNetLib.h"
#include <cstdio>
#include <WS2tcpip.h>

namespace NL = MyNetLib;

int main()
{
    NL::Autocleanup a;
    int ret = a.init();
    if (ret != 0) {
        printf("this is horosho in WSAStartup, %d\n", ::WSAGetLastError());
        return -1;
    }

    struct addrinfo * pGaiRes = NL::getUdpClientAddrInfo("localhost");
    if (pGaiRes == nullptr) {
        printf("this is horosho in getaddrinfo, %d\n", ::WSAGetLastError());
        return -1;
    }
    struct addrinfo* chosenAddrinfo = nullptr;
    struct addrinfo* tmp = pGaiRes;
    for (; tmp != nullptr; tmp = tmp->ai_next) {
        if (tmp->ai_family == AF_INET) {
            chosenAddrinfo = tmp;
            break;
        }
    }

    if (chosenAddrinfo == nullptr) {
        printf("cannot find suitable addrinfo\n");
        freeaddrinfo(pGaiRes);
        return -1;
    }

    return 0;
}
