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

