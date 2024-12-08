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

#ifdef _AYMMAP_WIN
#error unreachable
#endif

#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (-1)
#endif

namespace aymmap {
struct MemMapData {
    using handle_type = int;

    handle_type  handle_ = INVALID_HANDLE_VALUE;
    void *       p_data_ = nullptr;
    std::int64_t offset_{};
    std::size_t  length_{};
};

constexpr MemMapData::handle_type kInvalidHandle = INVALID_HANDLE_VALUE;

struct MemMapTraits {
    using handle_type = MemMapData::handle_type;

    static std::size_t pageSize() {
        return static_cast<std::size_t>(sysconf(_SC_PAGE_SIZE));
    }

    static std::int64_t alignPageSize(std::int64_t sz) {
        return sz & (~(pageSize() - 1));
    }

    static handle_type openFile(char const * path, AccessFlag access) {
        return open(path, bool(access & AccessFlag::_kWrite) ? O_RDWR : O_RDONLY);
    }

    static decltype(auto) closeFile(handle_type handle) {
        return close(handle);
    }

    static std::size_t fileSize(handle_type handle) {
        struct stat st;
        if (fstat(handle, &st) == -1) { return 0U; }
        return static_cast<std::size_t>(st.st_size);
    }

    /**
     * https://man7.org/linux/man-pages/man2/mmap.2.html
     */
    static bool map(MemMapData & d, AccessFlag access, std::size_t length, std::int64_t offset) {
        int prot{};
        int flags{};

        if (bool(access & AccessFlag::kRead)) { prot |= PROT_READ; }
        if (bool(access & AccessFlag::_kWrite)) { prot |= PROT_WRITE; }
        if (bool(access & AccessFlag::kExec)) { prot |= PROT_EXEC; }
        flags = bool(access & AccessFlag::kCopy) ? MAP_PRIVATE : MAP_SHARED;
        if (d.handle_ == kInvalidHandle) { flags |= MAP_ANONYMOUS; }

        int64_t aligned_offset = alignPageSize(offset);
        int64_t mapped_length  = offset - aligned_offset + length;

        void * p_map = mmap(NULL, mapped_length, prot, flags, d.handle_, aligned_offset);
        if (p_map == MAP_FAILED) { return false; }
        d.p_data_ = p_map;
        d.offset_ = offset;
        d.length_ = mapped_length;
        return true;
    }

    static bool unmap(MemMapData & d) {
        return munmap(d.p_data_, d.length_) != -1;
    }

    static bool lock(void * addr, std::size_t length) {
        return mlock(addr, length) != -1;
    }

    static bool unlock(void * addr, std::size_t length) {
        return munlock(addr, length) != -1;
    }

    /**
     * https://man7.org/linux/man-pages/man2/mprotect.2.html
     */
    static bool protect(void * addr, std::size_t length, ProtectFlag prot_flag) {
        int prot{};
        return mprotect(addr, length, prot) != -1;
    }

    static bool sync(void * addr, std::size_t length) {
        return msync(addr, length, MS_SYNC) != -1;
    }

    static bool remap(MemMapData & d, std::size_t new_length) {
        void * p_newmap = nullptr;
#ifdef MREMAP_MAYMOVE
        p_newmap = mremap(d.p_data_, d.length_, new_length, MREMAP_MAYMOVE);
#else
        p_newmap = mremap(d.p_data_, d.length_, new_length, 0);
#endif
        if (p_newmap == (void *)-1) { return false; }
        d.p_data_ = p_newmap;
        d.length_ = new_length;
        return true;
    }
};
}

