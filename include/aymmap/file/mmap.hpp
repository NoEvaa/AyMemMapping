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

#include <iterator>

#include "aymmap/global.hpp"
#include "aymmap/file/mman.hpp"
#include "aymmap/file/utils.hpp"

namespace aymmap {
#ifdef AYMMAP_ENABLE_MMAP_FILE_FRIEND
template <typename...> class MMapFileFriend;
#endif

template <typename ByteT,
    typename _TraitsT = MemMapTraits,
    typename _UtilsT  = FileUtils<_TraitsT>
>
class BasicMMapFile : public _UtilsT {
    static_assert(sizeof(ByteT) == sizeof(char));

#ifdef AYMMAP_ENABLE_MMAP_FILE_FRIEND
    template <typename...> friend class MMapFileFriend;
#endif

public:
    using byte_type      = ByteT;
    using pointer        = byte_type *;
    using const_pointer  = byte_type const *;
    using iterator       = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using traits_type = _TraitsT;
    using handle_type = typename traits_type::handle_type;
    using data_type   = typename traits_type::data_type;
    using path_type   = typename traits_type::path_type;
    using path_cref   = typename traits_type::path_cref;
    using size_type   = typename traits_type::size_type;
    using off_type    = typename traits_type::off_type;

    using utils_type = _UtilsT;
    using utils_type::_throwErrno;

    static constexpr size_type kInvalidSize = static_cast<size_type>(-1);

    BasicMMapFile() = default;
    ~BasicMMapFile() { unmap(); }

    BasicMMapFile(BasicMMapFile && ot) noexcept { _move(std::move(ot)); }
    BasicMMapFile & operator=(BasicMMapFile && ot) {
        if (isMapped()) [[unlikely]] { if (unmap()) { return *this; } }
        _move(std::move(ot));
        return *this;
    }

    errno_t map(path_cref, AccessFlag, size_type length = kInvalidSize, size_type offset = 0);
    errno_t anonMap(size_type length);

    template <typename FileT>
    errno_t fileMap(FileT file, AccessFlag flag, bool b_dup = true,
        size_type length = kInvalidSize, size_type offset = 0) {
        if (isMapped()) [[unlikely]] { if (auto en = unmap()) { return en; } }
        return _fileMap(utils_type::toFileHandle(file), flag, b_dup, length, offset);
    }

    errno_t unmap();
    errno_t flush();
    errno_t sync(size_type offset, size_type length);
    errno_t remap(AccessFlag, size_type length, size_type offset);
    errno_t resize(size_type new_length);
    errno_t lock();
    errno_t unlock();
    errno_t protect(AccessFlag);
    errno_t advise(AdviceFlag);

    bool isMapped() const noexcept { return bool(m_p_byte); }
    bool isAnon() const noexcept { return isMapped() && _isAnon(); }
    bool empty() const noexcept { return size() == 0; }

    size_type size() const noexcept { return m_length; }
    pointer       data() noexcept { return m_p_byte; }
    const_pointer data() const noexcept { return m_p_byte; }
    const_pointer c_str() const noexcept { return m_p_byte; }

    handle_type fileHandle() const noexcept { return m_data.file_handle_; }

    iterator       begin() noexcept { return data(); }
    const_iterator begin() const noexcept { return data(); }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator       end() noexcept { return data() + size(); }
    const_iterator end() const noexcept { return data() + size(); }
    const_iterator cend() const noexcept { return end(); }

    reverse_iterator       rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    reverse_iterator       rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    byte_type &       operator[](size_type i) noexcept { return m_p_byte[i]; }
    byte_type const & operator[](size_type i) const noexcept { return m_p_byte[i]; }

private:
    void _reset();
    void _move(BasicMMapFile && ot) noexcept {
        m_p_byte = std::exchange(ot.m_p_byte, nullptr);
        m_length = std::exchange(ot.m_length, 0);
        m_b_internal_file = std::exchange(ot.m_b_internal_file, false);
        m_data = std::move(ot.m_data);
    }

    bool _isAnon() const noexcept { return fileHandle() == kInvalidHandle; }

    errno_t _mapImpl(AccessFlag, size_type, off_type);
    errno_t _mapFileImpl(AccessFlag, size_type, size_type);
    errno_t _fileMap(handle_type, AccessFlag, bool, size_type, size_type);

    BasicMMapFile(BasicMMapFile const &) = delete;
    BasicMMapFile & operator=(BasicMMapFile const &) = delete;

private:
    data_type m_data;
    pointer   m_p_byte = nullptr;
    size_type m_length = 0;
    bool      m_b_internal_file = false;
};
using MMapFile = BasicMMapFile<char>;

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::map(path_cref ph,
    AccessFlag flag, size_type length, size_type offset) {
    if (isMapped()) [[unlikely]] { if (auto en = unmap()) { return en; } }

    // open file
    auto file_handle = traits_type::fileOpen(ph, flag);
    if (!traits_type::checkHandle(file_handle)) [[unlikely]] { return _throwErrno(false); }
    m_data.file_handle_ = file_handle;
    m_b_internal_file   = true;

    auto en = _mapFileImpl(flag, length, offset);
    if (en) { _reset(); }
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::anonMap(size_type length) {
    if (isMapped()) [[unlikely]] { if (auto en = unmap()) { return en; } }
    if (length == 0 || length == kInvalidSize) { return kEnoInviArgs; }
    m_data.file_handle_ = kInvalidHandle;
    auto en = _mapImpl(AccessFlag::kDefault, length, 0);
    if (en) { _reset(); }
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::_fileMap(handle_type file_handle,
    AccessFlag flag, bool b_dup, size_type length, size_type offset) {
    if (!traits_type::checkHandle(file_handle)) [[unlikely]] { return kEnoInviArgs; }

    // duplicate file handle
    if (b_dup) {
        auto dup_handle = traits_type::dupHandle(file_handle);
        if (!traits_type::checkHandle(dup_handle)) { return _throwErrno(false); }
        m_data.file_handle_ = dup_handle;
        m_b_internal_file   = true;
    } else {
        m_data.file_handle_ = file_handle;
    }

    auto en = _mapFileImpl(flag, length, offset);
    if (en) { _reset(); }
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::_mapFileImpl(
    AccessFlag flag, size_type length, size_type offset) {
    auto file_sz = utils_type::fileSize(m_data.file_handle_);

    if (length == kInvalidSize) {
        if (file_sz <= offset) [[unlikely]] { return kEnoInviArgs; }
        length = file_sz - offset;
    } else if (length == 0) [[unlikely]] {
        return kEnoInviArgs;
    } else if (utils_type::_mustToResize(flag)) {
        if (!traits_type::fileResize(m_data.file_handle_, length + offset)) {
            return _throwErrno(false);
        }
    } else if (length + offset > file_sz) {
        if (utils_type::_ableToResize(flag) && 
            traits_type::fileResize(m_data.file_handle_, length + offset)) {
            file_sz = length + offset;
        } else if (file_sz <= offset) [[unlikely]] {
            return kEnoInviArgs;
        } else if (file_sz - offset < length) {
            length = file_sz - offset;
        }
    }

    return _mapImpl(flag, length, (off_type)offset);
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::_mapImpl(
    AccessFlag flag, size_type length, off_type offset) {
    off_type  aligned_offset = utils_type::alignToPageSize(offset);
    size_type mapped_length  = size_type(offset - aligned_offset) + length;
    auto en = _throwErrno(traits_type::map(m_data, flag, mapped_length, aligned_offset));
    if (en) { return en; }
    m_length = length;
    offset  -= aligned_offset;
    m_p_byte = reinterpret_cast<pointer>(m_data.p_data_) + offset;
    return en;
}
 
template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::unmap() {
    if (!isMapped()) { return _throwErrno(true); }
    auto en = _throwErrno(traits_type::unmap(m_data));
    if (en) { return en; }
    _reset();
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::flush() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    if (isAnon()) [[unlikely]] { return kEnoMapIsAnon; }
    return _throwErrno(traits_type::sync(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::sync(
    size_type offset, size_type length) {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    if (isAnon()) [[unlikely]] { return kEnoMapIsAnon; }
    if (m_length <= offset) [[unlikely]] { return kEnoInviArgs; }
    if (m_length - offset < length) [[unlikely]] { length = m_length - offset; }
    return _throwErrno(traits_type::sync(m_p_byte + offset, length));
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::remap(
    AccessFlag flag, size_type length, size_type offset) {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    if (isAnon()) [[unlikely]] { return kEnoMapIsAnon; }

    if (!traits_type::unmap(m_data)) { return _throwErrno(false); }

    auto en = _mapFileImpl(flag, length, offset);
    if (en) { _reset(); }
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::resize(size_type new_length) {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    if (new_length == m_length) [[unlikely]] { return kEnoOk; }
    auto offset = m_data.length_ - m_length;
    auto mapped_length = new_length + offset;
    auto en = _throwErrno(traits_type::remap(m_data, mapped_length));
    if (m_data.p_data_) {
        m_p_byte = reinterpret_cast<pointer>(m_data.p_data_) + offset;
        m_length = m_data.length_ - offset;
    } else { _reset(); }
    return en;
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::lock() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _throwErrno(traits_type::lock(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::unlock() {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _throwErrno(traits_type::unlock(m_data.p_data_, m_data.length_));
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::protect(AccessFlag flag) {
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _throwErrno(traits_type::protect(m_data.p_data_, m_data.length_, flag));
}

template <typename T, typename T2, typename T3>
errno_t BasicMMapFile<T, T2, T3>::advise(AdviceFlag flag) {
#ifdef _AYMMAP_UNIMPL_ADVISE
    return kEnoUnimpl;
#else
    if (!isMapped()) [[unlikely]] { return kEnoUnmapped; }
    return _throwErrno(traits_type::advise(m_data.p_data_, m_data.length_, flag));
#endif
}

template <typename T, typename T2, typename T3>
void BasicMMapFile<T, T2, T3>::_reset() {
    m_p_byte = nullptr;
    m_length = 0;
    if (m_b_internal_file) {
        traits_type::fileClose(m_data.file_handle_);
        m_b_internal_file = false;
    } else {
        m_data.file_handle_ = kInvalidHandle;
    }
}

}

