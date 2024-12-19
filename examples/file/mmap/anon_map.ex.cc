/**
 * Copyright 2024 NoEvaa
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}

void printMap(MMapFile & mmfi) {
    std::cout << "Map size: " << mmfi.size() << std::endl;

    std::cout << "Content: ";
    for (auto iter = mmfi.begin(); iter != mmfi.end(); ++iter) {
        std::cout << *iter;
    }
    std::cout << std::endl;
}

int main() {
    MMapFile mmfi;
    if (auto en = mmfi.anonMap(10)) {
        std::cout << "Anon map failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }
    assert(mmfi.isMapped());
    assert(mmfi.isAnon());

    std::cout << "- Write..." << std::endl;
    char * s = mmfi.data();
    std::string text = "1234567890";
    std::copy_n(text.c_str(), text.size(), s);
    s[0] = '0';

    printMap(mmfi);

    std::cout << "- Map resize..." << std::endl;
    if (auto en = mmfi.resize(5)) {
        std::cout << "Resize failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }

    printMap(mmfi);

    if (auto en = mmfi.unmap()) {
        std::cout << "Unmap failed: [" << en << "] "
            << errMsg(en) << std::endl;
        return -1;
    }

    return 0;
}

