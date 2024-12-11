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
    ~BasicFileMap() { (void)unmap(); }
    BasicFileMap(BasicFileMap &&);
    BasicFileMap & operator=(BasicFileMap &&);
    BasicFileMap(BasicFileMap const &) = delete;
    BasicFileMap & operator=(BasicFileMap const &) = delete;

    bool isMapped() const noexcept { return bool(m_p_byte); }
    size_type size() const noexcept { return m_length; }

    pointer data() noexcept { return m_p_byte; }
    const_pointer data() const noexcept { return m_p_byte; }
    const_pointer c_str() const noexcept { return m_p_byte; }

    errno_type map(path_cref, AccessFlag);
    errno_type map(FILE *, AccessFlag);
    errno_type unmap();
    errno_type flush() { return _toErrno(traits_type::sync(m_data.p_data_, m_data.length_)); }
    errno_type resize();
    errno_type lock() { return _toErrno(traits_type::lock(m_data.p_data_, m_data.length_)); }
    errno_type unlock() { return _toErrno(traits_type::unlock(m_data.p_data_, m_data.length_)); }
    errno_type protect(AccessFlag flag)
        { return _toErrno(traits_type::protect(m_data.p_data_, m_data.length_, flag)); }
    errno_type advise(AdviceFlag flag)
        { return _toErrno(traits_type::advise(m_data.p_data_, m_data.length_, flag)); }

private:
    errno_type _toErrno(bool b) noexcept { return b ? errno_type(0) : traits_type::lastErrno(); } 
    handle_type _fileToHandle(FILE * fi) {
        return traits_type::filenoToHandle(traits_type::fileToFileno(fi));
    }

private:
    pointer   m_p_byte = nullptr;
    data_type m_data;
    size_type m_length = 0;
    off_type  m_offset = 0; // aligned
    bool      m_b_internal_file = false;
};

using MemMap = BasicFileMap<char>;

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::map(path_cref ph, AccessFlag flag) {}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::map(FILE * fi, AccessFlag flag) {}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::unmap() {
    if (!isMapped()) { return _toErrno(true); }
    auto en = _toErrno(traits_type::unmap(m_data));
    if (!en) { return en; }
    if (m_b_internal_file) {
        en = traits_type::closeFile(m_data.file_handle_);
    }
    m_p_byte            = nullptr;
    m_length            = 0;
    m_offset            = 0;
    m_b_internal_file   = false;
    m_data.file_handle_ = kInvalidHandle;
    return en;
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::resize() {}
}

