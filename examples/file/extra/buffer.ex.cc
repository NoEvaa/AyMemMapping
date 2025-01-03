/**
 * Copyright 2025 NoEvaa
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

#include <iostream>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

void bufWrite() {
    MMapFile mmfi;
    if (mmfi.map("test.txt", AccessFlag::kDefault, 100)) {
        throw;
    }
    MMapFileBuf mmfb(std::move(mmfi));

    assert(!mmfi.isMapped());
    assert(mmfb.size() == 100);
    assert(mmfb.tell() == 0);
    assert(mmfb.remaining() == 100);

    mmfb.writeByte('a');
    mmfb.write("123", 3);
    mmfb.writeView("456");

    mmfb.writeByte('\n');
    mmfb.writeByte('c');

    mmfb.seek(1, BufferPos::kBeg);
    assert(mmfb.tell() == 1);
    mmfb.writeByte('b');

    mmfb.flush();
}

void bufRead() {
    MMapFileBuf mmfb;
    mmfb.map("test.txt", AccessFlag::kReadOnly);

    assert(mmfb.size() == 100);
    assert(mmfb.tell() == 0);

    char c;
    mmfb.readByte(c);
    std::cout << "Read byte: " << c << std::endl;

    char cs[4];
    mmfb.read(cs, 3);
    cs[3] = 0;
    std::cout << "Read: " << cs << std::endl;

    std::cout << "Read view: " << mmfb.readView() << std::endl;

    mmfb.seek(0, BufferPos::kBeg);
    assert(mmfb.tell() == 0);
    std::cout << "Read line: " << mmfb.readline() << std::endl;

    mmfb.readByte(c);
    std::cout << "Read byte: " << c << std::endl;
}

int main() {
    bufWrite();
    bufRead();
    fs::remove("test.txt");
    return 0;
}

