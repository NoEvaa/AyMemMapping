/**
 * Copyright 2025 NoEvaa
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
#include <concepts>

#include "aymmap/global.hpp"
#include "aymmap/detail/endian.hpp"

namespace aymmap {
template <Endian _endian, typename BufT>
class BasicMMapStream {
    using self_type = BasicMMapStream;
public:
    using buffer_type   = BufT;
    using size_type     = typename buffer_type::size_type;
    using off_type      = typename buffer_type::off_type;
    using byte_type     = typename buffer_type::byte_type;
    using pointer       = typename buffer_type::pointer;
    using const_pointer = typename buffer_type::const_pointer;

    static constexpr auto npos = buffer_type::npos;

    enum class Status {
        kOk = 0,
        kReadFailed,
        kWriteFailed,
    };

    BasicMMapStream() = default;
    ~BasicMMapStream() = default;
    explicit BasicMMapStream(buffer_type && buf) noexcept
        : m_buf(std::move(buf)), m_stat(Status::kOk) {}
    BasicMMapStream(BasicMMapStream && ot) noexcept { _move(std::move(ot)); }
    BasicMMapStream & operator=(BasicMMapStream && ot) noexcept {
        _move(std::move(ot));
        return *this;
    }

    buffer_type & buffer() noexcept { return m_buf; }
    buffer_type const & buffer() const noexcept { return m_buf; }
    buffer_type setBuffer(buffer_type && buf = buffer_type{}) noexcept {
        setStatus();
        return std::exchange(m_buf, buf);
    }

    Status status() const noexcept { return m_stat; }
    void setStatus(Status stat = Status::kOk) noexcept { m_stat = stat; }

    size_type read(pointer data, size_type length = npos) noexcept {
        if (_check()) [[unlikely]] { return 0; }
        return m_buf.read(data, length);
    }

    size_type readByte(byte_type & data) noexcept {
        if (_check()) [[unlikely]] { return 0; }
        return m_buf.readByte(data);
    }

    size_type write(const_pointer data, size_type length) noexcept {
        if (_check()) [[unlikely]] { return 0; }
        return m_buf.write(data, length);
    }

    size_type writeByte(byte_type data) noexcept {
        if (_check()) [[unlikely]] { return 0; }
        return m_buf.writeByte(data);
    }

    void flush() noexcept { m_buf.flush(); }

    template <std::integral T>
    self_type & operator>>(T & i) {
        if (read(reinterpret_cast<pointer>(&i), sizeof(T)) != sizeof(T)) {
            setStatus(Status::kReadFailed);
            i = T{0};
        } else {
            i = autoFitEndian<_endian>(i);
        }
        return *this;
    }

    template <std::floating_point T>
    self_type & operator>>(T & f) {
        if (read(reinterpret_cast<pointer>(&f), sizeof(T)) != sizeof(T)) {
            setStatus(Status::kReadFailed);
            f = T{0.0};
        } else {
            f = autoFitEndian<_endian>(f);
        }
        return *this;
    }

    template <typename T>
    requires std::integral<T> || std::floating_point<T>
    self_type & operator<<(T n) {
        n = autoFitEndian<_endian>(n);
        if (write((pointer)&n, sizeof(T)) != sizeof(T)) {
            setStatus(Status::kWriteFailed);
        }
        return *this;
    }

    self_type & operator<<(self_type & (*_pfn)(self_type &)) { return _pfn(*this); }
    self_type & operator>>(self_type & (*_pfn)(self_type &)) { return _pfn(*this); }
    self_type & operator<<(std::function<self_type &(self_type &)> _fn) { return _fn(*this); }
    self_type & operator>>(std::function<self_type &(self_type &)> _fn) { return _fn(*this); }

private:
    void _move(BasicMMapStream && ot) noexcept {
        m_buf  = std::move(ot.m_buf);
        m_stat = std::exchange(ot.m_stat, Status::kOk);
    }

    bool _check() noexcept {
        if (status() != Status::kOk) [[unlikely]] { return true; }
        return false;
    }

    _AYMMAP_DISABLE_CLASS_COPY(BasicMMapStream)

private:
    buffer_type m_buf;
    Status      m_stat = Status::kOk;
};
}

