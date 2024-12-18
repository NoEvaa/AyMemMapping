#include <iostream>


#define AYMMAP_ENABLE_MMAP_FILE_FRIEND
#include "aymmap/aymmap.hpp"

using namespace aymmap;

struct print_length;

template <>
class aymmap::MMapFileFriend<print_length> {
public:
    static void call(aymmap::MMapFile & mmfi) {
        std::cout << "Mapped length: " << mmfi.m_length << std::endl;
    }
};

int main() {
    MMapFile mmfi;
    MMapFileFriend<print_length>::call(mmfi);
    return 0;
}

