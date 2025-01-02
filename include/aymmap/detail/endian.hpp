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

#include <cstdint>
#include <type_traits>
#include <concepts>
#include <bit>

namespace aymmap {
using Endian = std::endian;

namespace detail {
inline constexpr uint8_t byteswap(uint8_t n) noexcept { return n; }
inline constexpr uint16_t byteswap(uint16_t n) noexcept { return (n << 8) | (n >> 8); }
inline constexpr uint32_t byteswap(uint32_t n) noexcept {
    n = ((n << 8) & 0xff00ff00) | ((n >> 8) & 0xff00ff);
    return (n << 16) | (n >> 16);
}

inline constexpr uint64_t byteswap(uint64_t n) noexcept {
    n = ((n << 8)  & UINT64_C(0xff00ff00ff00ff00)) | ((n >> 8)  & UINT64_C(0xff00ff00ff00ff));
    n = ((n << 16) & UINT64_C(0xffff0000ffff0000)) | ((n >> 16) & UINT64_C(0xffff0000ffff));
    return (n << 32) | (n >> 32);
}

inline constexpr bool byteswap(bool n) noexcept { return n; }

template <std::integral T>
inline constexpr T byteswap(T n) noexcept {
    return T(byteswap(std::make_unsigned_t<T>(n)));
}

template <std::floating_point T>
inline constexpr T byteswap(T n) noexcept {
    using uint_type = std::conditional_t<std::is_same_v<T, double>, uint64_t, uint32_t>;
    static_assert(sizeof(T) == sizeof(uint_type));
    union {
        T         val1;
        uint_type val2;
    } x;
    x.val1 = n;
    x.val2 = byteswap(x.val2);
    return x.val1;
}
}

template <Endian tgt, typename T>
inline constexpr T autoFitEndian(T n) noexcept {
    if constexpr (tgt == Endian::native) {
        return n;
    } else {
        return detail::byteswap(n);
    }
}
}

