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
#include "testlib.h"
#include "aymmap/global.hpp"

using namespace aymmap;

TEST_CASE("access flag") {
    CHECK(AccessFlag::kRead & AccessFlag::kRead);
    CHECK(AccessFlag::kRead & AccessFlag::kWrite);
    CHECK(AccessFlag::kRead & AccessFlag::kWrite);
    CHECK((~AccessFlag::kRead) & AccessFlag::kWrite);
    CHECK(!bool(AccessFlag::kWrite & AccessFlag::kCopy));
    CHECK(!bool(AccessFlag::kWrite & AccessFlag::kExec));
    CHECK(!bool(AccessFlag::kCopy & AccessFlag::kExec));
    CHECK(AccessFlag::kWriteCopy == (AccessFlag::kCopy | AccessFlag::kWrite));
    CHECK(AccessFlag::kReadExec == (AccessFlag::kExec | AccessFlag::kRead));
    CHECK(AccessFlag::kWrite == (AccessFlag::kWriteCopy ^ AccessFlag::kCopy));
}

