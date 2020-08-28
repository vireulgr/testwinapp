#include "../MyNetLib/MyNetLib.h"
#include <cstdio>
#include <WS2tcpip.h>
#include <memory>
#include <locale>

namespace NL = MyNetLib;

//unsigned short const g_port = 27182;
char const * g_table1251[] = {
       "000\0", "001\0", "002\0", "003\0", "004\0", "005\0", "006\0", "007\0", "008\0", "009\0", 
       "\\n  \0", "011\0", "012\0", "013\0", "014\0", "015\0", "016\0", "017\0", "018\0", "019\0", 
       "020\0", "021\0", "022\0", "023\0", "024\0", "025\0", "026\0", "027\0", "028\0", "029\0", 
       "030\0", "031\0", " _ \0", "!  \0", "\"  \0", "#  \0", "$  \0", "%  \0", "&  \0", "'  \0", 
       "(  \0", ")  \0", "*  \0", "+  \0", ",  \0", "-  \0", ".  \0", "/  \0", "0  \0", "1  \0", 
/*5*/  "2  \0", "3  \0", "4  \0", "5  \0", "6  \0", "7  \0", "8  \0", "9  \0", ":  \0", ";  \0", 
       "< \0", "=  \0", ">  \0", "?  \0", "@  \0", "A  \0", "B  \0", "C  \0", "D  \0", "E  \0", 
       "F  \0", "G  \0", "H  \0", "I  \0", "J  \0", "K  \0", "L  \0", "M  \0", "N  \0", "O  \0", 
       "P  \0", "Q  \0", "R  \0", "S  \0", "T  \0", "U  \0", "V  \0", "W  \0", "X  \0", "Y  \0", 
       "Z  \0", "[  \0", "\\  \0", "]  \0", "^  \0", "___\0", "096\0", "a  \0", "b  \0", "c  \0", 
/*10*/ "d  \0", "e  \0", "f  \0", "g  \0", "h  \0", "105\0", "106\0", "107\0", "108\0", "109\0", 
       "110\0", "111\0", "112\0", "113\0", "114\0", "115\0", "116\0", "117\0", "118\0", "119\0", 
       "x  \0", "y  \0", "z  \0", "{  \0", "|  \0", "}  \0", "126\0", "127\0", "A  \0", "Б  \0", 
       "В  \0", "Г  \0", "Д  \0", "Е  \0", "Ж  \0", "З  \0", "И  \0", "Й  \0", "К  \0", "Л  \0", 
       "М  \0", "Н  \0", "О  \0", "П  \0", "Р  \0", "С  \0", "T  \0", "У  \0", "Ф  \0", "Х  \0", 
/*15*/ "Ц  \0", "Ч  \0", "Ш  \0", "Щ  \0", "Ъ  \0", "Ы  \0", "Ь  \0", "Э  \0", "Ю  \0", "Я  \0", 
       "а  \0", "б  \0", "в  \0", "г  \0", "д  \0", "е  \0", "ж  \0", "з  \0", "и  \0", "й  \0", 
       "к  \0", "л  \0", "м  \0", "н  \0", "о  \0", "п  \0", "\u2591  \0", "\u2592  \0", "\u2593  \0", "179\0", 
       "180\0", "181\0", "182\0", "183\0", "184\0", "185\0", "186\0", "187\0", "188\0", "189\0", 
       "190\0", "191\0", "192\0", "193\0", "194\0", "195\0", "196\0", "197\0", "198\0", "199\0", 
/*20*/ "200\0", "201\0", "202\0", "203\0", "204\0", "205\0", "206\0", "207\0", "208\0", "209\0", 
       "210\0", "211\0", "212\0", "213\0", "214\0", "215\0", "216\0", "217\0", "218\0", "219\0", 
       "220\0", "221\0", "222\0", "223\0", "р  \0", "с  \0", "т  \0", "у  \0", "ф  \0", "х  \0", 
       "ц  \0", "ч  \0", "ш  \0", "щ  \0", "ъ  \0", "ы  \0", "ь  \0", "э  \0", "ю  \0", "я  \0", 
       "Ё  \0", "ё  \0", "242\0", "243\0", "244\0", "245\0", "246\0", "247\0", "248\0", "249\0", 
/*25*/ "250\0", "251\0", "№  \0", "253\0", "254\0", "255\0", "256\0", "257\0", "258\0", "259\0", 
       "Unk\0"
};

void convertEnc(char* dest, int destLen, char const* src, int srcLen) {

}

char const * cp866ToCp1251(char c) {
    unsigned char idx = c;
    if (idx > 0 && idx < 260) {
        return g_table1251[idx];
    }
    return g_table1251[260];
}

int main()
{
    setlocale(LC_ALL, "Russian");
    NL::Autocleanup a;
    int ret = a.init();
    if (ret != 0) {
        ::printf("this is horosho in WSAStartup, %d\n", ::WSAGetLastError());
        return -1;
    }

    struct addrinfo * pGaiRes = NL::getUdpClientAddrInfo("localhost");
    if (pGaiRes == nullptr) {
        ::printf("this is horosho in getaddrinfo, %d\n", ::WSAGetLastError());
        return -1;
    }
    struct addrinfo* chosenAddrInfo = nullptr;
    struct addrinfo* tmp = pGaiRes;
    for (; tmp != nullptr; tmp = tmp->ai_next) {
        if (tmp->ai_family == AF_INET) {
            chosenAddrInfo = tmp;
            break;
        }
    }

    if (chosenAddrInfo == nullptr) {
        ::printf("cannot find suitable addrinfo\n");
        ::freeaddrinfo(pGaiRes);
        return -1;
    }

    SOCKET sendSock = ::socket(chosenAddrInfo->ai_family, chosenAddrInfo->ai_socktype, chosenAddrInfo->ai_protocol);
    if (sendSock == INVALID_SOCKET) {
        ::printf("this is horosho in socket, %d\n", ::WSAGetLastError());
        ::freeaddrinfo(pGaiRes);
        return -1;
    }

    struct sockaddr_in* saTo = (sockaddr_in*)chosenAddrInfo->ai_addr;
    wchar_t inetAddrBuf[128] = { 0 };
    ::InetNtopW(AF_INET, &(saTo->sin_addr), inetAddrBuf, 128);

    long long bufLen = 2048;
    std::unique_ptr<char[]> scanBuf = std::make_unique<char[]>(bufLen);
    std::unique_ptr<char[]> buf1251 = std::make_unique<char[]>(bufLen*4);
    while (true) {
            ::memset(scanBuf.get(), 0, bufLen);
            ::memset(buf1251.get(), 0, bufLen*4);
            ::fgets(scanBuf.get(), bufLen - 1, stdin);
            int buf1251Idx = 0;
            for (char * c = scanBuf.get(); *c != '\0'; c++) {
                char const * converted = cp866ToCp1251(*c);
                ::strcpy_s(buf1251.get() + buf1251Idx, 2048 - buf1251Idx - 1, converted);
                buf1251Idx += 3;
            }
            char const* ptr = buf1251.get();
            int len = buf1251Idx;
            //char const* ptr = scanBuf.get();
            //int len = strlen(ptr);
            ::printf("%s sending to %ws\n", ptr, inetAddrBuf);
        ::sendto(sendSock, ptr, len, 0, (sockaddr*)&saTo, sizeof(sockaddr_in));
    }

    return 0;
}
