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

#include <source_location>

#include "aymmap/detail/log.hpp"

namespace aymmap {
template <>
struct LogFmt<std::source_location> {
    std::ostream & output(std::ostream & ost, std::source_location const & v) {
        ost << '[' << v.file_name() << '(' << v.line() << ")]["
            << v.function_name() << ']';
        return ost;
    }
};
}

