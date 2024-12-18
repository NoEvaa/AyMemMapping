#include <iostream>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}

int main() {
    auto ph = fs::path("test.txt");

    MMapFile mmfi;
    if (auto en = mmfi.map(ph, AccessFlag::kDefault, 10)) {
        std::cout << "File map failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }
    assert(mmfi.isMapped());
    assert(!mmfi.isAnon());

    std::cout << "Map size: " << mmfi.size() << std::endl;

    char * s = mmfi.data();
    std::string text = "1234567890";
    std::copy_n(text.c_str(), text.size(), s);
    s[0] = '0';

    if (auto en = mmfi.resize(5)) {
        std::cout << "File resize failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }
    std::cout << "Map size after resize: " << mmfi.size() << std::endl;

    if (auto en = mmfi.flush()) {
        std::cout << "File flush failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }

    std::cout << "File content: ";
    for (auto iter = mmfi.begin(); iter != mmfi.end(); ++iter) {
        std::cout << *iter;
    }
    std::cout << std::endl;

    if (auto en = mmfi.unmap()) {
        std::cout << "File unmap failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }

    fs::remove(ph);
    return 0;
}

