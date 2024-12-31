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

int main() {

    int max_i = 10000000;
    int j = 0;
    using itt = uint32_t;
    {
        auto start = tt2();
        for (itt i = 0; i < max_i; ++i) {
            j = detail::byteswap(i);
        }
        auto end = tt2();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "a Time taken: " << duration.count() << " microseconds" << std::endl;
    }

    AYMMAP_DEBUG(1,2,3);
    AYMMAP_WARN(1,2,3);
    AYMMAP_ERROR(1,2,3);
    return 0;
}


