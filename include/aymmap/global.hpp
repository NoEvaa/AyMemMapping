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

#include "aymmap/config.h"

#define _AYMMAP_DECL_ENUM_OP(_enum_tp)                                                             \
inline constexpr _enum_tp operator|(_enum_tp lhs, _enum_tp rhs) {                                  \
    return static_cast<_enum_tp>(                                                                  \
        static_cast<std::underlying_type_t<_enum_tp>>(lhs) |                                       \
        static_cast<std::underlying_type_t<_enum_tp>>(rhs));                                       \
}                                                                                                  \
inline constexpr _enum_tp operator&(_enum_tp lhs, _enum_tp rhs) {                                  \
    return static_cast<_enum_tp>(                                                                  \
        static_cast<std::underlying_type_t<_enum_tp>>(lhs) &                                       \
        static_cast<std::underlying_type_t<_enum_tp>>(rhs));                                       \
}                                                                                                  \
inline constexpr _enum_tp operator~(_enum_tp e) {                                                  \
    return static_cast<_enum_tp>(~static_cast<std::underlying_type_t<_enum_tp>>(e));               \
}                                                                                                  \

namespace aymmap {
enum class AccessFlag : std::uint32_t {
    kRead  = 0x0001,
    kWrite = 0x0002 | kRead,
    kCopy  = 0x0004,
    kExec  = 0x0008,

    kDefault = kWrite,
};
_AYMMAP_DECL_ENUM_OP(AccessFlag)

}

#undef _AYMMAP_DECL_ENUM_OP

