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

#include "aymmap/mman.hpp"

namespace aymmap {
template <typename ByteT>
class BasicMemMap {
public:
    using byte_type   = ByteT;
    using traits_type = MemMapTraits;
    using data_type   = traits_type::data_type;
    using path_type   = traits_type::path_type;
    using path_cref   = traits_type::path_cref;

private:
    byte_type * m_p_str;
};
}

