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
    using byte_type      = ByteT;
    using pointer        = byte_type *;
    using const_pointer  = byte_type const *;
    using iterator       = pointer; 
    using const_iterator = const_pointer; 

    using traits_type = MemMapTraits;
    using data_type   = traits_type::data_type;
    using path_type   = traits_type::path_type;
    using path_cref   = traits_type::path_cref;
    using size_type   = traits_type::size_type;
    using off_type    = traits_type::off_type;

    BasicMemMap() {}
    ~BasicMemMap() {}

    void map();
    void unmap();
    void flush();
    void resize();
    void lock();
    void unlock();
    void protect();
    void advise();

    pointer data() { return m_p_byte; }
    const_pointer data() const { return m_p_byte; }
    const_pointer c_str() const { return m_p_byte; }

private:
    pointer   m_p_byte = nullptr;
    data_type m_data{};
    off_type  m_aligned_offset{};
};

using MemMap = BasicMemMap<char>;
}

