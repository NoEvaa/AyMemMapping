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

#include <cstdint>
#include <filesystem>
#include <system_error>

#include "aymmap/global.hpp"

namespace aymmap {
template <typename T>
struct BasicMemMapTraits {
    using data_type   = T;
    using handle_type = typename data_type::handle_type;
    using size_type   = std::size_t;
    using off_type    = std::int64_t;
    using path_type   = fs::path;
    using path_cref   = path_type const &;

    static std::error_code lastError();

    static off_type pageSize();

    static size_type fileSize(handle_type);
    static handle_type filenoToHandle(int fd);
    static handle_type openFile(path_cref, AccessFlag);
    static bool closeFile(handle_type);
    static bool removeFile(path_cref);
    static bool resizeFile(handle_type, size_type new_size);

    static bool map(data_type &, AccessFlag, size_type length, off_type offset);
    static bool unmap(data_type &);
    static bool remap(data_type &, size_type new_length);

    static bool sync(void *, size_type length);
    static bool lock(void *, size_type length);
    static bool unlock(void *, size_type length);
    static bool protect(void *, size_type length, ProtectFlag);
};
}

#ifdef _AYMMAP_WIN
#include "aymmap/detail/mman_win.tcc"
#else
#include "aymmap/detail/mman_unix.tcc"
#endif

namespace aymmap {
namespace detail {
inline std::int64_t alignToPageSize(std::int64_t i) {
    return i & (~(MemMapTraits::pageSize() - 1));
}
}
}

