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
#include <cassert>
#include <type_traits>
#include <filesystem>

#define AYMMAP_VERSION "0.0.0"

#include "aymmap/config.hpp"
#include "aymmap/detail/errno.hpp"
#include "aymmap/log.hpp"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__NT__)
#define _AYMMAP_WIN
#endif

#define _AYMMAP_DISABLE_CLASS_COPY(_class)                                                         \
    _class(_class const &) = delete;                                                               \
    _class & operator=(_class const &) = delete;

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
inline constexpr _enum_tp operator^(_enum_tp lhs, _enum_tp rhs) {                                  \
    return static_cast<_enum_tp>(                                                                  \
        static_cast<std::underlying_type_t<_enum_tp>>(lhs) ^                                       \
        static_cast<std::underlying_type_t<_enum_tp>>(rhs));                                       \
}                                                                                                  \
inline constexpr _enum_tp operator~(_enum_tp e) {                                                  \
    return static_cast<_enum_tp>(~static_cast<std::underlying_type_t<_enum_tp>>(e));               \
}                                                                                                  \

namespace aymmap {
namespace fs = std::filesystem;

enum class AccessFlag : std::uint32_t {
    kRead   = 0x0001,
    _kWrite = 0x0002,
    kWrite  = _kWrite | kRead,
    kCopy   = 0x0004,
    kExec   = 0x0008,
    kCreate = 0x0010,
    _kResize = 0x0020,
    kResize  = _kResize | _kWrite,
    kNoAccess = 0x0040,

    kDefault   = kWrite | kCreate,
    kReadOnly  = kRead,
    kReadWrite = kWrite,
    kWriteCopy = kWrite | kCopy,
    kReadExec  = kRead | kExec,
};
_AYMMAP_DECL_ENUM_OP(AccessFlag)

enum class AdviceFlag {
    kNormal = 0,
    kRandom,
    kSequential,
    kWillNeed,
    kDontNeed,
};

enum class BufferPos {
    kBeg = 0,
    kEnd,
    kCur
};
}

#undef _AYMMAP_DECL_ENUM_OP

