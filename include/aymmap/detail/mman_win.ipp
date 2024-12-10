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

#include "aymmap/detail/mman_unix.tcc"
#include "aymmap/global.hpp"

#ifndef _AYMMAP_WIN
#error unreachable
#endif

#include <windows.h>
#include <io.h>
#include <system_error>

namespace aymmap {
struct MemMapData {
    using handle_type = HANDLE;

    handle_type file_handle_ = INVALID_HANDLE_VALUE;
    handle_type map_handle_  = INVALID_HANDLE_VALUE;
    void *      p_data_      = nullptr;
    std::size_t length_{};
};

static MemMapData::handle_type const kInvalidHandle = INVALID_HANDLE_VALUE;
using MemMapTraits = BasicMemMapTraits<MemMapData>;

template <>
int MemMapTraits::lastErrno() {
    return GetLastError();
}

template <>
MemMapTraits::off_type MemMapTraits::pageSize() {
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);
    return static_cast<off_type>(si.dwAllocationGranularity);
}

template <>
bool MemMapTraits::checkHandle(handle_type handle) {
    return (handle != kInvalidHandle) && (handle != NULL);
}

template <>
MemMapTraits::size_type MemMapTraits::fileSize(handle_type handle) {
    LARGE_INTEGER file_sz;
    if(::GetFileSizeEx(handle, &file_sz) == 0) { return 0U; }
    return static_cast<size_type>(file_sz.QuadPart);
}

template <>
MemMapTraits::handle_type MemMapTraits::filenoToHandle(int fd) {
    return (handle_type)_get_osfhandle(fd);
}

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
 */
template <>
MemMapTraits::handle_type MemMapTraits::openFile(path_cref ph, AccessFlag access) {
    DWORD access_mode = bool(access & AccessFlag::_kWrite) ?
        GENERIC_READ | GENERIC_WRITE : GENERIC_READ;
    DWORD create_mode = bool(access & AccessFlag::kCreate) ? OPEN_ALWAYS : OPEN_EXISTING;
    return ::CreateFileW(ph.c_str(), access_mode, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
                         create_mode, FILE_ATTRIBUTE_NORMAL, 0);
}

template <>
bool MemMapTraits::closeFile(handle_type handle) {
    return ::CloseHandle(handle);
}

template <>
bool MemMapTraits::removeFile(path_cref ph) {
    return ::DeleteFileW(ph.c_str());
}

template <>
bool MemMapTraits::resizeFile(handle_type handle, size_type new_size) {
    LARGE_INTEGER li;
    li.QuadPart = new_size;
    if (!::SetFilePointerEx(handle, li, NULL, FILE_BEGIN)) {
        return false;
    }
    return ::SetEndOfFile(handle);
}

namespace detail {
inline DWORD int64High(auto i) noexcept { return i >> 32; }
inline DWORD int64Low(auto i) noexcept { return i & 0xffffffff; }
}

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingw
 * https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile
 */
template <>
bool MemMapTraits::map(data_type & d, AccessFlag access, size_type length, off_type offset) {
    DWORD prot{};

    if (bool(access & AccessFlag::kCopy)) { prot = PAGE_WRITECOPY; }
    else if (bool(access & AccessFlag::_kWrite)) { prot = PAGE_READWRITE; }
    else { prot = PAGE_READONLY; }
    if (bool(access & AccessFlag::kExec)) { prot <<= 4; }

    const auto map_handle = ::CreateFileMappingW(d.file_handle_, 0, prot,
        detail::int64High(length), detail::int64Low(length), 0);
    if (!checkHandle(map_handle)) { return false; }

    prot = bool(access & AccessFlag::_kWrite) ? FILE_MAP_WRITE : FILE_MAP_READ;
    if (bool(access & AccessFlag::kCopy)) { prot |= FILE_MAP_COPY; }
    if (bool(access & AccessFlag::kExec)) { prot |= FILE_MAP_EXECUTE; }

    void * p_map = ::MapViewOfFile(map_handle, prot,
        detail::int64High(offset), detail::int64Low(offset), length);
    if (p_map == nullptr) {
        ::CloseHandle(map_handle);
        return false;
    }
    d.p_data_     = p_map;
    d.map_handle_ = map_handle;
    d.length_     = length;
    return true;
}

template <>
bool MemMapTraits::unmap(data_type & d) {
    if (!::UnmapViewOfFile(d.p_data_)) { return false; }
    ::CloseHandle(d.map_handle_);
    d.p_data_     = nullptr;
    d.map_handle_ = kInvalidHandle;
    d.length_     = 0;
    return true;
}

template <>
bool MemMapTraits::remap(data_type &, size_type) { return false; }

template <>
bool MemMapTraits::sync(void * addr, size_type length) {
    return ::FlushViewOfFile(addr, length);
}

template <>
bool MemMapTraits::lock(void * addr, size_type length) {
    return ::VirtualLock(addr, length);
}

template <>
bool MemMapTraits::unlock(void * addr, size_type length) {
    return ::VirtualUnlock(addr, length);
}

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect
 */
template <>
bool MemMapTraits::protect(void * addr, size_type length, AccessFlag access) {
    DWORD prot{};

    if (bool(access & AccessFlag::kNoAccess)) { prot = PAGE_NOACCESS; }
    else if (bool(access & AccessFlag::kCopy)) { prot = PAGE_WRITECOPY; }
    else if (bool(access & AccessFlag::_kWrite)) { prot = PAGE_READWRITE; }
    else { prot = PAGE_READONLY; }
    if (bool(access & AccessFlag::kExec)) { prot <<= 4; }

    return ::VirtualProtect(addr, length, prot, 0);
}

template <>
bool MemMapTraits::advise(void *, size_type, AdviceFlag) { return false; }
}

