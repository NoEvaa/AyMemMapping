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

#include "aymmap/config.hpp"
#include "aymmap/detail/log.hpp"
#include "aymmap/detail/log_fmt_time.hpp"
#include "aymmap/detail/log_fmt_src_loc.hpp"

#define AYMMAP_LOG(...)   _AYMMAP_LOG(__VA_ARGS__)
#define AYMMAP_WARN(...)  _AYMMAP_LOG(AYMMAP_LOG_TIME, "[WARN] ", __VA_ARGS__)
#define AYMMAP_ERROR(...) _AYMMAP_LOG(AYMMAP_LOG_TIME, "[ERROR] ", __VA_ARGS__)
#define AYMMAP_DEBUG(...) _AYMMAP_LOG_DEBUG(AYMMAP_LOG_TIME, AYMMAP_LOG_SRC_LOC, "[DEBUG] ", __VA_ARGS__)

#define AYMMAP_LOG_SRC_LOC std::source_location::current()
#define AYMMAP_LOG_TIME    aymmap::LogTime{}

#ifdef _AYMMAP_ENABLE_LOG
#define _AYMMAP_LOG(...) aymmap::detail::logOutput(__VA_ARGS__)
#else
#define _AYMMAP_LOG(...) static_cast<void>(0)
#endif

#ifdef _DEBUG
#define _AYMMAP_LOG_DEBUG(...) _AYMMAP_LOG(__VA_ARGS__)
#else
#define _AYMMAP_LOG_DEBUG(...) static_cast<void>(0)
#endif

