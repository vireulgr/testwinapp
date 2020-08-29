#include "../MyNetLib/MyNetLib.h"
#include <cstdio>
#include <WS2tcpip.h>
#include <memory>
#include <locale>

namespace NL = MyNetLib;

//unsigned short const g_port = 27182;

/* symbol in codepage      | 128 to 175 inclusive                     175 |                             */
/* symbol in codepage      |                                              || 224 to 252 inclusive  252 |*/
/* index in string         |                                              || 48                        |*/
/*                         v                                              vv                           v*/
char const * g_cyrillic = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяЁёЄєЇїЎў°∙·√№";

char const * g_table1251[] = {
          "\0",   "\\1",   "\\2",   "\\3",   "\\4",   "\\5",   "\\6",   "\\7",    "\b",    "\t",
          "\n",    "\v",    "\f",    "\r",  "\\14",  "\\15",  "\\16",  "\\17",  "\\18",  "\\19",
        "\\20",  "\\21",  "\\22",  "\\23",  "\\24",  "\\25",  "\\26",  "\\27",  "\\28",  "\\29",
        "\\30",  "\\31",     "_",     "!",    "\"",     "#",     "$",     "%",     "&",     "'",
           "(",     ")",     "*",     "+",     ",",     "-",     ".",     "/",     "0",     "1",
/*5*/      "2",     "3",     "4",     "5",     "6",     "7",     "8",     "9",     ":",     ";",
           "<",     "=",     ">",     "?",     "@",     "A",     "B",     "C",     "D",     "E",
           "F",     "G",     "H",     "I",     "J",     "K",     "L",     "M",     "N",     "O",
           "P",     "Q",     "R",     "S",     "T",     "U",     "V",     "W",     "X",     "Y",
           "Z",     "[",    "\\",     "]",     "^",     " ",     "`",     "a",     "b",     "c",
/*10*/     "d",     "e",     "f",     "g",     "h",     "i",     "j",     "k",     "l",     "m",
           "n",     "o",     "p",     "q",     "r",     "s",     "t",     "u",     "v",     "w",
           "x",     "y",     "z",     "{",     "|",     "}",     "~", "\\127",     "A",     "Б",
           "В",     "Г",     "Д",     "Е",     "Ж",     "З",     "И",     "Й",     "К",     "Л",
           "М",     "Н",     "О",     "П",     "Р",     "С",     "T",     "У",     "Ф",     "Х",
/*15*/     "Ц",     "Ч",     "Ш",     "Щ",     "Ъ",     "Ы",     "Ь",     "Э",     "Ю",     "Я",
           "а",     "б",     "в",     "г",     "д",     "е",     "ж",     "з",     "и",     "й",
           "к",     "л",     "м",     "н",     "о",     "п", "\\176", "\\177", "\\178", "\\179",
       "\\180", "\\181", "\\182", "\\183", "\\184", "\\185", "\\186", "\\187", "\\188", "\\189",
       "\\190", "\\191", "\\192", "\\193", "\\194", "\\195", "\\196", "\\197", "\\198", "\\199",
/*20*/ "\\200", "\\201", "\\202", "\\203", "\\204", "\\205", "\\206", "\\207", "\\208", "\\209",
       "\\210", "\\211", "\\212", "\\213", "\\214", "\\215", "\\216", "\\217", "\\218", "\\219",
       "\\220", "\\221", "\\222", "\\223",     "р",     "с",     "т",     "у",     "ф",     "х",
           "ц",     "ч",     "ш",     "щ",     "ъ",     "ы",     "ь",     "э",     "ю",     "я",
           "Ё",     "ё",     "Є",     "є",     "Ї",     "ї",     "Ў",     "ў",     "°", "\\249",
/*25*/ "\\250", "\\251",     "№", "\\253", "\\254", "\\255", "\\256", "\\257", "\\258", "\\259",
         "Unk"
};


void convertEnc(char* dest, char const* src) {

    int destIdx = 0;
    for (char const * c = src; *c != '\0'; c++) {
        unsigned char const curCharVal = *c;
        //printf("c is %hhu\n", curCharVal);
        if (curCharVal < 127) {
            dest[destIdx] = curCharVal;
            destIdx += 1;
        }
        else if (curCharVal >= 128 && curCharVal <= 175) {
            dest[destIdx] = g_cyrillic[curCharVal - 128];
            destIdx += 1;
        }
        else if (curCharVal >= 224 && curCharVal <= 252) {
            dest[destIdx] = g_cyrillic[curCharVal - 224 + 48];
            destIdx += 1;
        }
        else {
            puts("converting");
            char const* converted = (curCharVal > 0 && curCharVal < 260)
                                  ? g_table1251[curCharVal]
                                  : g_table1251[260];
            size_t strLen = ::strnlen_s(converted, 6);
            ::strcpy_s(dest + destIdx, 2048 - destIdx - 1, converted);
            destIdx += strLen;
        }
    }
}

int main()
{
    //char * oldLocale = setlocale(LC_ALL, "Russian");
    //printf("oldLocale   %s\n", oldLocale);

    //char const * oldLocale = setlocale(LC_CTYPE, nullptr);
    //printf("oldLocale %s\n", oldLocale);
    //if (oldLocale != nullptr && 0 == strcmp(oldLocale, "C")) {
    //    puts("old locale is C locale");
    //}
    //setlocale(LC_ALL, "Russian");
    //char const * newLocale = setlocale(LC_ALL, "Russian");
    //printf("newLocale %s\n", newLocale);
    //if (newLocale != nullptr && nullptr != strstr(newLocale, "Russian")) {
    //    puts("NEW locale is Russian locale");
    //}

    NL::Autocleanup a;
    int ret = a.init();
    if (ret != 0) {
        ::printf("error in WSAStartup, %d\n", ::WSAGetLastError());
        return -1;
    }

    struct addrinfo * pGaiRes = NL::getUdpClientAddrInfo("localhost");
    if (pGaiRes == nullptr) {
        ::printf("error in getaddrinfo, %d\n", ::WSAGetLastError());
        return -1;
    }
    struct addrinfo* tmp = pGaiRes;
    for (; tmp != nullptr; tmp = tmp->ai_next) {
        if (tmp->ai_family == AF_INET) break;
    }

    if (tmp == nullptr) {
        ::printf("cannot find suitable addrinfo\n");
        ::freeaddrinfo(pGaiRes);
        return -1;
    }

    SOCKET sendSock = ::socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
    if (sendSock == INVALID_SOCKET) {
        ::printf("error in socket, %d\n", ::WSAGetLastError());
        ::freeaddrinfo(pGaiRes);
        return -1;
    }

    struct sockaddr_in saTo = *(sockaddr_in*)tmp->ai_addr;
    wchar_t inetAddrBuf[128] = { 0 };
    ::InetNtopW(AF_INET, &(saTo.sin_addr), inetAddrBuf, 128);

    NL::printAddrInfo(tmp);
	unsigned short port = ::ntohs(saTo.sin_port);
	::printf("Sending to %ls:%hu\n", inetAddrBuf, port);

    long long bufLen = 2048;
    std::unique_ptr<char[]> scanBuf = std::make_unique<char[]>(bufLen);
    std::unique_ptr<char[]> buf1251 = std::make_unique<char[]>(bufLen*4);
    while (true) {
        ::memset(scanBuf.get(), 0, bufLen);

        ::fgets(scanBuf.get(), bufLen - 1, stdin);

        char const* ptr = buf1251.get();
        if (false) {
            ::memset(buf1251.get(), 0, bufLen*4);
            convertEnc(buf1251.get(), scanBuf.get());
            ptr = buf1251.get();
        }
        else {
            ptr = scanBuf.get();
        }
        int len = strlen(ptr);

        ::sendto(sendSock, ptr, len, 0, (struct sockaddr*)&saTo, sizeof(sockaddr_in));
    }

    return 0;
}
/* vim: set expandtab tabstop=4 shiftwidth=4 : */
