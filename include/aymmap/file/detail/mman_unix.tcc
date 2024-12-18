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

#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "aymmap/file/mman.hpp"

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (-1)
#endif

namespace aymmap {
using FileHandle = int;
static constexpr FileHandle kInvalidHandle = INVALID_HANDLE_VALUE;

template <> class FileHandleConverter<FileHandle> {
public:
    static FileHandle convert(FileHandle handle) noexcept {
        return handle;
    }
};

template <> class FileHandleConverter<FILE *> {
public:
    static FileHandle convert(FILE * fi) {
        if (!fi) [[unlikely]] { return kInvalidHandle; }
        return FileHandleConverter<int>::convert(fileno(fi));
    }
};

struct MemMapData {
    using handle_type = FileHandle;
    using size_type   = std::size_t;
    using off_type    = std::int64_t;

    handle_type file_handle_ = kInvalidHandle;
    void *      p_data_      = nullptr;
    size_type   length_{};
    off_type    offset_{};

    MemMapData() = default;
    ~MemMapData() = default;

    MemMapData & operator=(MemMapData && ot) noexcept {
        file_handle_ = std::exchange(ot.file_handle_, kInvalidHandle);
        p_data_ = std::exchange(ot.p_data_, nullptr);
        length_ = std::exchange(ot.length_, 0);
        offset_ = std::exchange(ot.offset_, 0);
        return *this;
    }

    MemMapData(MemMapData &&) = delete;
    MemMapData(MemMapData const &) = delete;
    MemMapData & operator=(MemMapData const &) = delete;
};
using MemMapTraits = BasicMemMapTraits<MemMapData>;

template <>
errno_t MemMapTraits::lastErrno() { return errno; }

template <>
MemMapTraits::off_type MemMapTraits::pageSize() {
    return static_cast<off_type>(::sysconf(_SC_PAGE_SIZE));
}

template <>
bool MemMapTraits::checkHandle(handle_type handle) {
    return (handle != kInvalidHandle);
}

template <>
MemMapTraits::handle_type MemMapTraits::dupHandle(handle_type handle) {
    return fcntl(handle, F_DUPFD_CLOEXEC, 0);
}

template <>
MemMapTraits::size_type MemMapTraits::fileSize(handle_type handle) {
    struct stat st;
    if (::fstat(handle, &st) == -1) { return 0U; }
    return static_cast<std::size_t>(st.st_size);
}

template <>
MemMapTraits::handle_type MemMapTraits::fileOpen(path_cref ph, AccessFlag access) {
    int mode = bool(access & AccessFlag::_kWrite) ? O_RDWR : O_RDONLY;
    if (bool(access & AccessFlag::kCreate)) { mode |= O_CREAT; }
    return ::open(ph.c_str(), mode, 0777);
}

template <>
bool MemMapTraits::fileClose(handle_type handle) {
    return ::close(handle) == 0;
}

template <>
bool MemMapTraits::fileResize(handle_type handle, size_type new_size) {
    return ::ftruncate(handle, new_size) == 0;
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
    if (d.file_handle_ == kInvalidHandle) { flags |= MAP_ANONYMOUS; }

    void * p_map = ::mmap(NULL, length, prot, flags, d.file_handle_, offset);
    if (p_map == MAP_FAILED) { return false; }
    d.p_data_ = p_map;
    d.length_ = length;
    d.offset_ = offset;
    return true;
}

template <>
bool MemMapTraits::unmap(data_type & d) {
    if (d.p_data_) [[likely]] {
        if (::munmap(d.p_data_, d.length_) == -1) { return false; }
    }
    d.p_data_ = nullptr;
    d.length_ = 0;
    d.offset_ = 0;
    return true;
}

template <>
bool MemMapTraits::remap(data_type & d, size_type new_length) {
    auto const new_file_sz = size_type(d.offset_) + new_length;
    if (d.file_handle_ != kInvalidHandle && !fileResize(d.file_handle_, new_file_sz)) {
        return false;
    }

    void * p_new_data = nullptr;
#ifdef MREMAP_MAYMOVE
    p_new_data = ::mremap(d.p_data_, d.length_, new_length, MREMAP_MAYMOVE);
#else
    p_new_data = ::mremap(d.p_data_, d.length_, new_length, 0);
#endif
    if (p_new_data == MAP_FAILED) { return false; }
    d.p_data_ = p_new_data;
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
bool MemMapTraits::protect(void * addr, size_type length, AccessFlag access) {
    int prot{};

    if (bool(access & AccessFlag::kNoAccess)) {
        prot = PROT_NONE;
    } else {
        if (bool(access & AccessFlag::kRead)) { prot |= PROT_READ; }
        if (bool(access & AccessFlag::_kWrite)) { prot |= PROT_WRITE; }
        if (bool(access & AccessFlag::kExec)) { prot |= PROT_EXEC; }
    }

    return ::mprotect(addr, length, prot) != -1;
}

/**
 * [madvise(2)](http://man7.org/linux/man-pages/man2/madvise.2.html)
 */
template <>
bool MemMapTraits::advise(void * addr, size_type length, AdviceFlag adv_flag) {
    int flag{};
    switch (adv_flag) {
        case AdviceFlag::kNormal: flag = MADV_NORMAL;
        case AdviceFlag::kRandom: flag = MADV_RANDOM;
        case AdviceFlag::kSequential: flag = MADV_SEQUENTIAL;
        case AdviceFlag::kWillNeed: flag = MADV_WILLNEED;
        case AdviceFlag::kDontNeed: flag = MADV_DONTNEED;
        default: return false;
    }
    return ::madvise(addr, length, flag) != -1;
}
}

