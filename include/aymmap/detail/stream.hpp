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
#include <type_traits>

#include "aymmap/detail/endian.hpp"

namespace aymmap {
template <Endian _endian, typename BufT>
class BasicMMapStream {
public:
    using buffer_type   = BufT;
    using size_type     = typename buffer_type::size_type;
    using off_type      = typename buffer_type::off_type;
    using byte_type     = typename buffer_type::byte_type;
    using pointer       = typename buffer_type::pointer;
    using const_pointer = typename buffer_type::const_pointer;

    using buffer_type::npos;

    enum class Status {
        kOk = 0,
    };

    BasicMMapStream() = default;
    ~BasicMMapStream() = default;
    explicit BasicMMapStream(buffer_type && buf) noexcept : m_buf(std::move(buf)), m_st(Status::kOk) {}
    BasicMMapStream(BasicMMapStream && ot) noexcept { _move(std::move(ot)); }
    BasicMMapStream & operator=(BasicMMapStream && ot) noexcept {
        _move(std::move(ot));
        return *this;
    }

    buffer_type & getBuffer() noexcept { return m_buf; }
    buffer_type const & getBuffer() const noexcept { return m_buf; }
    void pushBuffer(buffer_type && buf) noexcept { m_buf = std::move(buf); } 
    buffer_type popBuffer() noexcept { return std::exchange(m_buf, buffer_type{}); }

    size_type read(pointer data, size_type length = npos) noexcept { return m_buf.read(data, length); }
    size_type write(const_pointer data, size_type length) noexcept { return m_buf.write(data, length); }

    template <typename Fn, typename = std::invoke_result_t<Fn, BasicMMapStream &>>
    BasicMMapStream & operator<<(Fn __pf) {
        __pf(*this);
        return *this;
    }

private:
    void _move(BasicMMapStream && ot) noexcept {
        m_buf = std::move(ot.m_buf);
        m_st  = std::exchange(ot.m_st, Status::kOk);
    }

    BasicMMapStream(BasicMMapStream const &) = delete;
    BasicMMapStream & operator=(BasicMMapStream const &) = delete;

private:
    buffer_type m_buf;
    Status      m_st = Status::kOk;
};
}

