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

    using traits_type = _Traits;
    using handle_type = typename traits_type::handle_type;
    using data_type   = typename traits_type::data_type;
    using path_type   = typename traits_type::path_type;
    using path_cref   = typename traits_type::path_cref;
    using size_type   = typename traits_type::size_type;
    using off_type    = typename traits_type::off_type;
    using errno_type  = typename traits_type::errno_type;

    static constexpr size_type kInvalidSize = static_cast<size_type>(-1);

    static errno_type const kEnoOk = errno_type(0);
    static errno_type const kEnoUnimpl = errno_type(-1);
    static errno_type const kEnoUnmapped = errno_type(-2);

    BasicFileMap() = default;
    ~BasicFileMap() { (void)unmap(); }
    BasicFileMap(BasicFileMap &&);
    BasicFileMap & operator=(BasicFileMap &&);
    BasicFileMap(BasicFileMap const &) = delete;
    BasicFileMap & operator=(BasicFileMap const &) = delete;

    errno_type map(path_cref, AccessFlag, size_type length = kInvalidSize, off_type offset = 0);
    errno_type map(FILE *, AccessFlag);
    errno_type anonMap(size_type length);

    errno_type unmap();
    errno_type flush();
    errno_type remap(AccessFlag, size_type length, size_type offset);
    errno_type resize(size_type new_length);
    errno_type lock();
    errno_type unlock();
    errno_type protect(AccessFlag);
    errno_type advise(AdviceFlag);

    size_type size() const noexcept { return m_length; }
    pointer data() noexcept { return m_p_byte; }
    const_pointer data() const noexcept { return m_p_byte; }
    const_pointer c_str() const noexcept { return m_p_byte; }

    bool isMapped() const noexcept { return bool(m_p_byte); }
    bool isAnon() const noexcept { return isMapped() && (m_data.file_handle_ == kInvalidHandle); }

private:
    errno_type _toErrno(bool b) noexcept { return b ? kEnoOk : traits_type::lastErrno(); } 
    handle_type _fileToHandle(FILE * fi) {
        return traits_type::filenoToHandle(traits_type::fileToFileno(fi));
    }
    void _clear() noexcept {
        m_p_byte            = nullptr;
        m_length            = 0;
        m_offset            = 0;
        m_b_internal_file   = false;
        m_data.file_handle_ = kInvalidHandle;
    }
    bool _mapImpl(AccessFlag, size_type, off_type);

private:
    pointer   m_p_byte = nullptr;
    data_type m_data;
    size_type m_length = 0;
    off_type  m_offset = 0;
    bool      m_b_internal_file = false;
};

using FileMap = BasicFileMap<char>;

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::map(path_cref ph,
    AccessFlag flag, size_type length, off_type offset) {
    if (isMapped()) [[unlikely]] {
        auto en = unmap();
        if (en) { return en; }
    }
    auto file_handle = traits_type::fileOpen(ph, flag);
    if (!traits_type::checkHandle(file_handle)) [[unlikely]] { return _toErrno(false); }

    m_data.file_handle_ = file_handle;
    

    m_b_internal_file = true;
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::map(FILE * fi, AccessFlag flag) {}

template <typename T, typename T2>
bool BasicFileMap<T, T2>::_mapImpl(
    AccessFlag flag, size_type length, off_type offset) {
    off_type  aligned_offset = detail::alignToPageSize(offset);
    size_type mapped_length  = size_type(offset - aligned_offset) + length;
    if (!traits_type::map(m_data, flag, mapped_length, aligned_offset)) {
        return false;
    }
    m_length = length;
    m_offset = offset - aligned_offset;
    m_p_byte = reinterpret_cast<pointer>(m_data.p_data_) + m_offset;
    return true;
}
 
template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::unmap() {
    if (!isMapped()) { return _toErrno(true); }
    auto en = _toErrno(traits_type::unmap(m_data));
    if (!en) { return en; }
    if (m_b_internal_file) {
        en = traits_type::closeFile(m_data.file_handle_);
    }
    _clear();
    return en;
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::flush() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _toErrno(traits_type::sync(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::resize(size_type) {}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::lock() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _toErrno(traits_type::lock(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::unlock() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _toErrno(traits_type::unlock(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::protect(AccessFlag flag) {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _toErrno(traits_type::protect(m_data.p_data_, m_data.length_, flag));
}

template <typename T, typename T2>
BasicFileMap<T, T2>::errno_type BasicFileMap<T, T2>::advise(AdviceFlag flag) {
#ifdef _AYMMAP_UNIMPL_ADVISE
    return kEnoUnimpl;
#else
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _toErrno(traits_type::advise(m_data.p_data_, m_data.length_, flag));
#endif
}
}

