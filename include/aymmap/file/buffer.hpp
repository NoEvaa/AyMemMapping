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

#include <string_view>

#include "aymmap/file/mmap.hpp"

namespace aymmap {
template <typename FileT = MMapFile>
class BasicMMapFileBuf {
public:
    using file_type     = FileT;
    using size_type     = typename file_type::size_type;
    using byte_type     = typename file_type::byte_type;
    using pointer       = typename file_type::pointer;
    using const_pointer = typename file_type::const_pointer;

    using view_type = std::basic_string_view<byte_type>;

    static constexpr auto npos = static_cast<size_type>(-1);

    BasicMMapFileBuf(file_type && fi) noexcept : m_file(std::move(fi)), m_pos(0) {}
    ~BasicMMapFileBuf() = default;
    BasicMMapFileBuf(BasicMMapFileBuf && ot) noexcept { _move(std::move(ot)); }
    BasicMMapFileBuf & operator=(BasicMMapFileBuf && ot) noexcept {
        _move(std::move(ot));
        return *this;
    }

    file_type &       getMMap() noexcept { return m_file; }
    file_type const & getMMap() const noexcept { return m_file; }

    bool isEOF() const noexcept { return m_pos < m_file.size(); }
    size_type tell() const noexcept { return m_pos; }
    size_type seek(size_type pos) noexcept {
        m_pos = pos > m_file.size() ? m_file.size() : pos;
        return m_pos;
    }

    void flush() { m_file.flush(); }

    view_type read(size_type length) noexcept {
        if (isEOF()) [[unlikely]] { return view_type{}; }
        if (m_pos + length >= m_file.size()) {
            length = m_file.size() - m_pos;
        }
        auto p = m_file.data() + m_pos;
        m_pos += length;
        return view_type{p, length};
    }

    view_type readline(byte_type sep = '\n') noexcept {
        if (isEOF()) [[unlikely]] { return view_type{}; }
        size_type length = 0;
        auto p = m_file.data() + m_pos;
        while (length + m_pos < m_file.size()) {
            if (*(p + length) == sep) {
                ++length;
                break;
            }
            ++length;
        }
        m_pos += length;
        return view_type{p, length};
    }

    byte_type readByte() noexcept {
        return isEOF() ? static_cast<byte_type>(0) : m_file.data()[m_pos++];
    }

    size_type write(const_pointer bytes, size_type length) noexcept {
        if (isEOF()) [[unlikely]] { return 0; }
        if (m_pos + length >= m_file.size()) {
            length = m_file.size() - m_pos;
        }
        std::copy_n(bytes, length, m_file.data() + m_pos);
        m_pos += length;
        return length;
    }

    size_type writeByte(byte_type byte) noexcept {
        if (isEOF()) [[unlikely]] { return 0; }
        m_file.data()[m_pos++] = byte;
        return 1;
    }
    
private:
    void _move(BasicMMapFileBuf && ot) noexcept {
        m_file = std::move(ot.m_file);
        m_pos = std::move(ot.m_pos);
    }

    BasicMMapFileBuf(BasicMMapFileBuf const &) = delete;
    BasicMMapFileBuf & operator=(BasicMMapFileBuf const &) = delete;

private:
    file_type m_file;
    size_type m_pos = 0;
};
using MMapFileBuf = BasicMMapFileBuf<MMapFile>;
}

