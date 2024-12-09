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
#include <system_error>
#include <sys/mman.h>
#include <sys/stat.h>

#include "aymmap/mman.hpp"

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (-1)
#endif

namespace aymmap {
struct MemMapData {
    using handle_type = int;

    handle_type handle_ = INVALID_HANDLE_VALUE;
    void *      p_data_ = nullptr;
    std::size_t length_{};
};

constexpr MemMapData::handle_type kInvalidHandle = INVALID_HANDLE_VALUE;
using MemMapTraits = BasicMemMapTraits<MemMapData>;

template <>
std::error_code MemMapTraits::lastError() {
    std::error_code ec;
    ec.assign(errno, std::system_category());
    return ec;
}

template <>
MemMapTraits::size_type MemMapTraits::pageSize() {
    return static_cast<size_type>(::sysconf(_SC_PAGE_SIZE));
}

template <>
MemMapTraits::size_type MemMapTraits::fileSize(handle_type handle) {
    struct stat st;
    if (::fstat(handle, &st) == -1) { return 0U; }
    return static_cast<std::size_t>(st.st_size);
}

template <>
MemMapTraits::handle_type MemMapTraits::filenoToHandle(int fd) {
    return fd;
}

template <>
MemMapTraits::handle_type MemMapTraits::openFile(path_cref ph, AccessFlag access) {
    int mode = bool(access & AccessFlag::_kWrite) ? O_RDWR : O_RDONLY;
    if (bool(access & AccessFlag::kCreate)) {
        mode |= O_CREAT;
    }
    return ::open(ph.c_str(), mode, 0666);
}

template <>
bool MemMapTraits::closeFile(handle_type handle) {
    ::close(handle);
    return true;
}

template <>
bool MemMapTraits::removeFile(path_cref ph) {
    ::remove(ph.c_str());
    return true;
}

template <>
bool MemMapTraits::resizeFile(handle_type handle, size_type new_size) {
    ::ftruncate(handle, new_size);
    return true;
}

/**
 * [mmap(2)](http://man7.org/linux/man-pages/man2/mmap.2.html)
 */
template <>
bool MemMapTraits::map(data_type & d, AccessFlag access, size_type length, off_type offset) {
    int prot{};
    int flags{};

    if (bool(access & AccessFlag::kRead)) { prot |= PROT_READ; }
    if (bool(access & AccessFlag::_kWrite)) { prot |= PROT_WRITE; }
    if (bool(access & AccessFlag::kExec)) { prot |= PROT_EXEC; }
    flags = bool(access & AccessFlag::kCopy) ? MAP_PRIVATE : MAP_SHARED;
    if (d.handle_ == kInvalidHandle) { flags |= MAP_ANONYMOUS; }

    void * p_map = ::mmap(NULL, length, prot, flags, d.handle_, offset);
    if (p_map == MAP_FAILED) { return false; }
    d.p_data_ = p_map;
    d.length_ = length;
    return true;
}

template <>
bool MemMapTraits::unmap(data_type & d) {
    if (::munmap(d.p_data_, d.length_) == -1) { return false; }
    d.p_data_ = nullptr;
    d.length_ = 0;
    return true;
}

template <>
bool MemMapTraits::remap(data_type & d, size_type new_length) {
    void * p_newmap = nullptr;
#ifdef MREMAP_MAYMOVE
    p_newmap = ::mremap(d.p_data_, d.length_, new_length, MREMAP_MAYMOVE);
#else
    p_newmap = ::mremap(d.p_data_, d.length_, new_length, 0);
#endif
    if (p_newmap == MAP_FAILED) { return false; }
    d.p_data_ = p_newmap;
    d.length_ = new_length;
    return true;
}

template <>
bool MemMapTraits::sync(void * addr, size_type length) {
    return ::msync(addr, length, MS_SYNC) != -1;
}

template <>
bool MemMapTraits::lock(void * addr, size_type length) {
    return ::mlock(addr, length) != -1;
}

template <>
bool MemMapTraits::unlock(void * addr, size_type length) {
    return ::munlock(addr, length) != -1;
}

/**
 * [mprotect(2)](http://man7.org/linux/man-pages/man2/mprotect.2.html)
 */
template <>
bool MemMapTraits::protect(void * addr, size_type length, ProtectFlag prot_flag) {
    int prot{};
    return ::mprotect(addr, length, prot) != -1;
}
}

