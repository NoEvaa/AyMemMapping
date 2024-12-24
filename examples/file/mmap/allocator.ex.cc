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
#include <unordered_map>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

static std::unordered_map<char const *, MMapFile> s_mem_pool;

char * myAlloc(size_t length) {
    if (length == 0) [[unlikely]] {
        return nullptr;
    }
    MMapFile mmfi;
    mmfi.anonMap(length);
    char * p_mem = mmfi.data();
    assert(p_mem);
    s_mem_pool.insert({p_mem, std::move(mmfi)});
    return p_mem;
}

void myDealloc(char const * p_mem) {
    s_mem_pool.erase(p_mem);
}

char * myRealloc(char const * p_mem, size_t length) {
    if (s_mem_pool.find(p_mem) == s_mem_pool.end()) [[unlikely]] {
        return nullptr;
    }
    auto & mmfi = s_mem_pool[p_mem];
    if (mmfi.resize(length)) {
        myDealloc(p_mem);
        return nullptr;
    }
    return mmfi.data();
}

int main() {
    char * p_mem = myAlloc(10);

    std::string text = "1234567890";
    std::copy_n(text.c_str(), text.size(), p_mem);
    p_mem[9] = 0;
    std::cout << "alloc 10: " << p_mem << std::endl;

    myRealloc(p_mem, 5);
    p_mem[4] = 0;
    std::cout << "alloc 5: " << p_mem << std::endl;

    char * p_mem2 = myAlloc(1 << 30); // 1GB
    
    std::cout << "pool size: " << s_mem_pool.size() << std::endl;
    myDealloc(p_mem);
    myDealloc(p_mem2);
    std::cout << "pool size: " << s_mem_pool.size() << std::endl;
    return 0;
}

