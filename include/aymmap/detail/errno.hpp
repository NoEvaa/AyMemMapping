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

namespace aymmap {
using errno_t = int;

constexpr errno_t kEnoOk = errno_t(0);
constexpr errno_t kEnoUnimpl = errno_t(-1);
constexpr errno_t kEnoInviArgs = errno_t(-2);
constexpr errno_t kEnoUnmapped = errno_t(-3);
constexpr errno_t kEnoMapIsAnon = errno_t(-4);
}

