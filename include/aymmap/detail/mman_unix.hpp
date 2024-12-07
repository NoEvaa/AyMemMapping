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
#pragma once

#include "aymmap/global.hpp"

#include <bits/sysconf.h>
#include <cstdint>
#include <cstdio>

#ifdef _AYMMAP_WIN
#error unreachable
#endif

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (-1)
#endif

namespace aymmap {
using FileHandle = int;

constexpr FileHandle kInvalidHandle = INVALID_HANDLE_VALUE;

namespace detail {
inline int getPageSize() {
    return sysconf(_SC_PAGE_SIZE);
}
}

struct MemMapData {
    void *      p_data_ = nullptr;
    FileHandle  handle_ = kInvalidHandle;
    std::size_t length_{};
    std::size_t mapped_length_{};
};

struct MemMapTraits {
    /**
     * https://man7.org/linux/man-pages/man2/mmap.2.html
     */
    bool map(MemMapData & d, int64_t offset) {
        size_t length{};
        size_t mapped_length{};
        int prot{};
        int flags{};
        off_t aligned_offset{};
        d.p_data_ = mmap(NULL, mapped_length, prot, flags, d.handle_, aligned_offset);
        
        return true;
    }

    bool unmap(MemMapData & d) {
        if (!d.p_data_)
        {
            return true;
        }
        return munmap(d.p_data_, d.length_) != -1;
    }

    void protect(MemMapData & d) {}
    void lock(MemMapData & d) {}
    void unlock(MemMapData & d) {}
};
}

