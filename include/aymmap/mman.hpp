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

#include "aymmap/global.hpp"

namespace aymmap {
template <typename T>
struct BasicMemMapTraits {
    using data_type   = T;
    using handle_type = typename data_type::handle_type;
    using size_type   = typename data_type::size_type;
    using off_type    = typename data_type::off_type;

    using path_type   = fs::path;
    using path_cref   = path_type const &;

    using errno_type = int;

    static errno_type lastErrno();
    static off_type pageSize();

    static bool checkHandle(handle_type);

    static size_type fileSize(handle_type);
    static int fileToFileno(FILE *);
    static handle_type filenoToHandle(int fd);
    static handle_type fileOpen(path_cref, AccessFlag);
    static bool fileClose(handle_type);
    static bool fileRemove(path_cref);
    static bool fileResize(handle_type, size_type new_size);

    static bool map(data_type &, AccessFlag, size_type length, off_type offset);
    static bool unmap(data_type &);
    static bool remap(data_type &, size_type new_length);

    static bool sync(void *, size_type length);
    static bool lock(void *, size_type length);
    static bool unlock(void *, size_type length);
    static bool protect(void *, size_type length, AccessFlag);
    static bool advise(void *, size_type length, AdviceFlag);
};
}

#ifdef _AYMMAP_WIN
#include "aymmap/detail/mman_win.ipp"
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

