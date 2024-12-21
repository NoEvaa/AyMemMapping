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

#include <iostream>
#include <type_traits>

namespace aymmap {
template <typename T>
struct LogFmt {
    std::ostream & output(std::ostream & ost, T const & v) {
        return (ost << v);
    }
};

struct LogTime {};

namespace detail {
template <typename... Ts>
void logOutput(Ts... vs) {
    (LogFmt<std::decay_t<Ts>>().output(std::cerr, vs), ...) << std::endl;
}
}
}

