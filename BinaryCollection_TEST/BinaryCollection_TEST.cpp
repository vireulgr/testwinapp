#include <cstdio>
#include <list>
#include <vector>
#include "../BinaryCollection/BinaryCollection.h"

struct AStruct {
    char m_strBuf[512];
    int m_int;
    bool m_bool;
    double m_double;
};

namespace BC = BinaryCollection;

int main()
{
    std::list<AStruct> testList;

    std::vector<unsigned char> a = BC::ListToBin<AStruct>(testList);

    BC::ListBinaryCollection<AStruct> lbc = BC::BinToListCollection<AStruct>(a);

    return 0;
}

