#include <filesystem>
#include <iostream>
#include <system_error>
#include "aymmap/aymmap.hpp"
#include "aymmap/global.hpp"

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}

int main() {
    return 0;
}


