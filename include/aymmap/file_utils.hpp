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

#include "aymmap/mman.hpp"

namespace aymmap {
template <typename _TraitsT = MemMapTraits>
class FileUtils {
public:
    using traits_type = _TraitsT;
    using handle_type = typename traits_type::handle_type;
    using path_cref   = typename traits_type::path_cref;
    using off_type    = typename traits_type::off_type;
    using errno_type  = typename traits_type::errno_type;

    static constexpr errno_type kEnoOk = errno_type(0);

    template <typename FileT>
    static handle_type toFileHandle(FileT fi) { return FileHandleConverter<FileT>::convert(fi); }

    static errno_type removeFile(path_cref ph) { return _toErrno(traits_type::fileRemove(ph)); }

    static errno_type _toErrno(bool b) noexcept { return b ? kEnoOk : traits_type::lastErrno(); } 

    static off_type pageSize() { return traits_type::pageSize(); }
    static off_type alignToPageSize(off_type i) { return i & (~(pageSize() - 1)); }
};
}

