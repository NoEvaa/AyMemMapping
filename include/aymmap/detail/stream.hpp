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

#include <algorithm>

#include "aymmap/detail/endian.hpp"

namespace aymmap {
template <Endian _endian, typename BufT>
class BasicMMapStream {
public:
    using buffer_type = BufT;

    BasicMMapStream() = default;
    ~BasicMMapStream() = default;
    explicit BasicMMapStream(buffer_type && buf) noexcept : m_buf(std::move(buf)) {}
    BasicMMapStream(BasicMMapStream && ot) noexcept { m_buf = std::move(ot.m_buf); }
    BasicMMapStream & operator=(BasicMMapStream && ot) noexcept {
        m_buf = std::move(ot.m_buf);
        return *this;
    }

    buffer_type & getBuffer() noexcept { return m_buf; }
    buffer_type const & getBuffer() const noexcept { return m_buf; }
    void pushBuffer(buffer_type && buf) noexcept { m_buf = std::move(buf); } 
    buffer_type popBuffer() noexcept { return std::exchange(m_buf, buffer_type{}); }

private:
    BasicMMapStream(BasicMMapStream const &) = delete;
    BasicMMapStream & operator=(BasicMMapStream const &) = delete;

private:
    buffer_type m_buf;
};
}

