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

#include "aymmap/file_map.hpp"

namespace aymmap {
template <typename _FileMapT = FileMap>
struct BasicFileUtils {
    using file_map_type = _FileMapT;
    using traits_type   = typename file_map_type::traits_type;
    using handle_type   = typename file_map_type::handle_type;
    using errno_type    = typename file_map_type::errno_type;
    using path_cref     = typename file_map_type::path_cref;

    static handle_type toFileHandle(int fd) {
        return traits_type::filenoToHandle(fd);
    }

    static handle_type toFileHandle(FILE * fi) {
        return toFileHandle(traits_type::fileToFileno(fi));
    }

    static errno_type removeFile(path_cref ph) {
        return traits_type::fileRemove(ph) ?
            file_map_type::kEnoOk : traits_type::lastErrno();
    }

};
using FileUtils = BasicFileUtils<FileMap>;
}

