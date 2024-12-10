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
template <typename ByteT, typename _Traits = MemMapTraits>
class BasicFileMap {
public:
    using byte_type      = ByteT;
    using pointer        = byte_type *;
    using const_pointer  = byte_type const *;
    using iterator       = pointer;
    using const_iterator = const_pointer;

    using errno_type = int;

    using traits_type = _Traits;
    using handle_type = typename traits_type::handle_type;
    using data_type   = typename traits_type::data_type;
    using path_type   = typename traits_type::path_type;
    using path_cref   = typename traits_type::path_cref;
    using size_type   = typename traits_type::size_type;
    using off_type    = typename traits_type::off_type;

    BasicFileMap() = default;
    ~BasicFileMap() { unmap(); }
    BasicFileMap(BasicFileMap &&);
    BasicFileMap & operator=(BasicFileMap &&);
    BasicFileMap(BasicFileMap const &) = delete;
    BasicFileMap & operator=(BasicFileMap const &) = delete;

    errno_type map(path_cref);
    errno_type map(FILE *);
    errno_type unmap();
    errno_type flush();
    errno_type resize();
    errno_type lock();
    errno_type unlock();
    errno_type protect();
    errno_type advise();

    bool isMapped() const noexcept;
    size_type size() const noexcept;

    pointer data() noexcept { return m_p_byte; }
    const_pointer data() const noexcept { return m_p_byte; }
    const_pointer c_str() const noexcept { return m_p_byte; }

private:
    pointer   m_p_byte = nullptr;
    data_type m_data{};
    off_type  m_aligned_offset{};
};

using MemMap = BasicFileMap<char>;
}

