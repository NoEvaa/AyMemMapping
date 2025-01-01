#include <concepts>
#include <cstdint>
#include <iostream>
#include <source_location>
#include <system_error>
#include "aymmap/aymmap.hpp"
#include "aymmap/detail/endian.hpp"

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}


#include <chrono>

auto tt2() {
    return std::chrono::high_resolution_clock::now();
}

auto ttd(auto d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

int main() {
    double dd = 1.23;
    std::cout << *(double*)(uint64_t*)&dd << std::endl;
    std::cout << *((double*)(uint64_t*)&dd) << std::endl;

    bool a = true;
    std::cout << autoFitEndian<Endian::native>(a)<< std::endl;
    std::cout << std::integral<bool> << std::endl;

    AYMMAP_DEBUG(1,2,3);
    AYMMAP_WARN(1,2,3);
    AYMMAP_ERROR(1,2,3);
    return 0;
}


