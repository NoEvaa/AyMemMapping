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

#include <type_traits>
#include <bit>

namespace aymmap {
using std::endian;

namespace detail {
inline constexpr uint8_t byteswap(uint8_t n) noexcept { return n; }
inline constexpr uint16_t byteswap(uint16_t n) noexcept { return (n << 8) | (n >> 8); }
inline constexpr uint32_t byteswap(uint32_t n) noexcept {
    n = ((n << 8) & 0xff00ff00) | ((n >> 8) & 0xff00ff);
    return (n << 16) | (n >> 16);
}
inline constexpr uint64_t byteswap(uint64_t n) noexcept {
    return ((n & UINT64_C(0x00000000000000ff)) << 56)
        | ((n & UINT64_C(0x000000000000ff00)) << 40)
        | ((n & UINT64_C(0x0000000000ff0000)) << 24)
        | ((n & UINT64_C(0x00000000ff000000)) << 8)
        | ((n & UINT64_C(0x000000ff00000000)) >> 8)
        | ((n & UINT64_C(0x0000ff0000000000)) >> 24)
        | ((n & UINT64_C(0x00ff000000000000)) >> 40)
        | ((n & UINT64_C(0xff00000000000000)) >> 56);
}

template <typename T>
inline constexpr T byteswap(T n) noexcept {
    return T(byteswap(std::make_unsigned_t<T>(n)));
}
}

template <endian tgt>
inline constexpr auto autoFixEndian(auto n) noexcept {
    if constexpr (tgt == endian::native) {
        return n;
    } else {
        return detail::byteswap(n);
    }
}
}

