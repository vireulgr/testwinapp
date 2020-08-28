#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include <cstdio>
#include "..\MyNetLib\MyNetLib.h"

int main()
{
    MyNetLib::Autocleanup a;
    int res = a.init();
    if (res != 0) {
        printf("error in wsaint; gle: %d\n", GetLastError());
        return -1;
    }
    return 0;
}
