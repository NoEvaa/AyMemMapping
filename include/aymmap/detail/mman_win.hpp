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

#include "aymmap/global.hpp"

#ifndef _AYMMAP_WIN
#error unreachable
#endif

namespace aymmap {
struct MemMapData {
    using handle_type = HANDLE;

    handle_type  handle_ = INVALID_HANDLE_VALUE;
    void *       p_data_ = nullptr;
    std::int64_t offset_{};
    std::size_t  length_{};
};

constexpr MemMapData::handle_type kInvalidHandle = INVALID_HANDLE_VALUE;

struct MemMapTraits {
    using handle_type = MemMapData::handle_type;

    static auto pageSize() {
    }

    static std::int64_t alignPageSize(std::int64_t sz) {
        return sz & (~(pageSize() - 1));
    }

    static handle_type openFile(char const * path, AccessFlag access) {
    }

    static decltype(auto) closeFile(handle_type handle) {
    }

    static std::size_t fileSize(handle_type handle) {
    }

    /**
     */
    static bool map(MemMapData & d, AccessFlag access, std::size_t length, std::int64_t offset) {
        return true;
    }

    static bool unmap(MemMapData & d) {
    }

    static bool lock(void * addr, std::size_t length) {
    }

    static bool unlock(void * addr, std::size_t length) {
    }

    /**
     */
    static bool protect(void * addr, std::size_t length, ProtectFlag prot_flag) {
    }
};
}


