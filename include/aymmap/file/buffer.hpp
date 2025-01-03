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

#include <cstring>
#include <string_view>
#include <utility>

#include "aymmap/file/mmap.hpp"

namespace aymmap {
template <typename FileT = MMapFile>
class BasicMMapFileBuf {
public:
    using file_type     = FileT;
    using size_type     = typename file_type::size_type;
    using off_type      = typename file_type::off_type;
    using byte_type     = typename file_type::byte_type;
    using pointer       = typename file_type::pointer;
    using const_pointer = typename file_type::const_pointer;

    using view_type = std::basic_string_view<byte_type>;

    static constexpr auto npos = static_cast<size_type>(-1);

    static_assert(sizeof(byte_type) == 1);

    BasicMMapFileBuf() = default;
    ~BasicMMapFileBuf() = default;
    explicit BasicMMapFileBuf(file_type && fi) noexcept : m_file(std::move(fi)), m_pos(0) {}
    BasicMMapFileBuf(BasicMMapFileBuf && ot) noexcept { _move(std::move(ot)); }
    BasicMMapFileBuf & operator=(BasicMMapFileBuf && ot) noexcept {
        _move(std::move(ot));
        return *this;
    }

    file_type & file() noexcept { return m_file; }
    file_type const & file() const noexcept { return m_file; }
    file_type setFile(file_type && fi = file_type{}) noexcept {
        m_pos = 0;
        return std::exchange(m_file, fi);
    }

    template <typename... Ts>
    auto map(Ts... args) {
        m_pos = 0;
        return m_file.map(std::forward<Ts>(args)...);
    }

    bool isEOF() const noexcept { return m_pos >= size(); }
    size_type size() const noexcept { return m_file.size(); }
    size_type tell() const noexcept { return m_pos; }
    size_type remaining() const noexcept { return tell() < size() ? size() - tell() : 0; }

    size_type seek(off_type offset, BufferPos whence = BufferPos::kCur) noexcept {
        m_pos = _getPos(offset, whence);
        return m_pos;
    }

    bool flush() noexcept { return m_file.flush() == kEnoOk; }

    size_type read(pointer data, size_type length = npos) noexcept {
        assert(data);
        if (isEOF()) [[unlikely]] { return 0; }
        if (size() - m_pos < length) { length = size() - m_pos; }
        std::memcpy(data, m_file.data() + m_pos, length);
        m_pos += length;
        return length;
    }

    size_type readByte(byte_type & data) noexcept {
        if (isEOF()) [[unlikely]] {
            data = byte_type{0};
            return 0;
        }
        data = m_file.data()[m_pos++];
        return 1;
    }

    view_type readView(size_type length = npos) noexcept {
        if (isEOF()) [[unlikely]] { return view_type{}; }
        if (size() - m_pos < length) { length = size() - m_pos; }
        auto p = m_file.data() + m_pos;
        m_pos += length;
        return view_type{p, length};
    }

    view_type readline(byte_type sep = '\n') noexcept {
        if (isEOF()) [[unlikely]] { return view_type{}; }
        size_type length = 0;
        auto p = m_file.data() + m_pos;
        auto const max_len = size() - m_pos;
        while (length < max_len) {
            if (*(p + length) == sep) {
                ++length;
                break;
            }
            ++length;
        }
        m_pos += length;
        return view_type{p, length};
    }

    size_type _write(const_pointer data, size_type length) noexcept {
        assert(data);
        std::memcpy(m_file.data() + m_pos, data, length);
        m_pos += length;
        return length;
    }

    size_type write(const_pointer data, size_type length) noexcept {
        if (isEOF()) [[unlikely]] { return 0; }
        if (size() - m_pos < length) { length = size() - m_pos; }
        return _write(data, length);
    }

    size_type writeByte(byte_type byte) noexcept {
        if (isEOF()) [[unlikely]] { return 0; }
        m_file.data()[m_pos++] = byte;
        return 1;
    }

    size_type writeView(view_type view) noexcept {
        return write(view.data(), view.size());
    }

private:
    void _move(BasicMMapFileBuf && ot) noexcept {
        m_file = std::move(ot.m_file);
        m_pos  = std::exchange(ot.m_pos, 0);
    }

    size_type _getPos(off_type offset, BufferPos whence) noexcept {
        switch (whence) {
        case BufferPos::kBeg:
            if (offset <= 0) [[unlikely]] { return 0; }
            if (offset > size()) { return size(); }
            return (size_type)offset;
        case BufferPos::kEnd:
            if (offset >= 0) [[unlikely]] { return size(); }
            if (-offset >= size()) { return 0; }
            return size() + offset;
        case BufferPos::kCur:
            [[fallthrough]];
        default:
            if (offset <= 0) {
                if (-offset >= m_pos) { return 0; }
                return m_pos + offset;
            }
            if (m_pos >= size()) [[unlikely]] { return size(); }
            if (size() - m_pos <= offset) { return size(); }
            return m_pos + offset;
        }
    }

    _AYMMAP_DISABLE_CLASS_COPY(BasicMMapFileBuf)

private:
    file_type m_file;
    size_type m_pos = 0;
};
using MMapFileBuf = BasicMMapFileBuf<MMapFile>;
}

