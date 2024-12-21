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

#include <chrono>
#include <ctime>
#include <iomanip>

#include "aymmap/detail/log.hpp"

namespace aymmap {
template <>
struct LogFmt<LogTime> {
    std::ostream & output(std::ostream & ost, LogTime v) {
        auto now   = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        ost << std::put_time(std::localtime(&now_c), "[%Y-%m-%d %H:%M:%S]");
        return ost;
    }
};
}

