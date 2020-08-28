#pragma once
#include <vector>
//#include <list>
//#include <map>

namespace std {
template <class T, class U>
class map;

template <class T>
class list;
}

class Command {
	int cmdCode;
	int dataSize;

};

namespace BinaryCollection {
	template <class T>
	class ListBinaryCollection {

		ListBinaryCollection();

		std::vector<unsigned char> m_data;
	};

	template <class T, class U>
	std::vector<unsigned char> MapToBin(std::map<T, U> &);

	template <class T>
	std::vector<unsigned char> VectorToBin(std::vector<T> &);

	template <class T>
	std::vector<unsigned char> ListToBin(std::list<T> &);

	template <class T>
	ListBinaryCollection<T> BinToListCollection(std::vector<unsigned char>);


}
