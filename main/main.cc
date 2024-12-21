#include <iostream>
#include <source_location>
#include <system_error>
#include "aymmap/aymmap.hpp"

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}

int main() {
    AYMMAP_DEBUG(1,2,3);
    AYMMAP_WARN(1,2,3);
    AYMMAP_ERROR(1,2,3);
    return 0;
}


