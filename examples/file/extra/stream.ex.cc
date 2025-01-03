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

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

MMapFileStream & operator<<(MMapFileStream & mmfs, std::string const & s) {
    std::uint64_t sz = std::uint64_t(s.size());
    mmfs << sz;
    if (mmfs.write(s.c_str(), sz) != sz) {
        mmfs.setStatus(MMapFileStream::Status::kWriteFailed);
    }
    return mmfs;
}

MMapFileStream & operator>>(MMapFileStream & mmfs, std::string & s) {
    std::uint64_t sz;
    mmfs >> sz;
    s.resize(sz);
    if (mmfs.read(s.data(), sz) != sz) {
        mmfs.setStatus(MMapFileStream::Status::kReadFailed);
        s = "";
    }
    return mmfs;
}

MMapFileStream & endl(MMapFileStream & mmfs) {
    std::cout << "endl" << std::endl;
    mmfs.writeByte('\n');
    mmfs.flush();
    return mmfs;
}

struct ReadLine {
    void operator()(MMapFileStream & mmfs) {
        auto sv = mmfs.buffer().readline();
        std::cout << "l size: " << sv.size() << std::endl;
        std::cout << "l: " << sv << std::endl;
    }
};

void streamWrite() {
    MMapFile mmfi;
    if (mmfi.map("test.txt", AccessFlag::kDefault, 100)) {
        throw;
    }
    MMapFileBuf mmfb(std::move(mmfi));
    MMapFileStream mmfs(std::move(mmfb));

    assert(!mmfi.isMapped());
    assert(mmfb.isEOF());
    assert(mmfs.status() == MMapFileStream::Status::kOk);

    mmfs << int(6) << double(2.3);
    mmfs << "hello, world";

    mmfs.write("12 45", 5);
    mmfs << endl;

    mmfs.write("ab de", 5);
    mmfs << endl;

    assert(mmfs.status() == MMapFileStream::Status::kOk);
}

void streamRead() {
    MMapFileStream mmfs;
    mmfs.map("test.txt", AccessFlag::kReadOnly);

    int i;
    double f;

    mmfs >> i >> f;
    assert(mmfs.status() == MMapFileStream::Status::kOk);
    std::cout << "i: " << i << "\nf: " << f << std::endl;

    std::string s;
    mmfs >> s;
    std::cout << "s: " << s << std::endl;

    mmfs >> ReadLine();

    std::string_view sv;
    mmfs >> [&sv](MMapFileStream & _mmfs) { 
        sv = _mmfs.buffer().readline();
    };
    std::cout << "l: " << sv << std::endl;

    assert(mmfs.status() == MMapFileStream::Status::kOk);
}

int main() {
    streamWrite();
    streamRead();
    fs::remove("test.txt");
    return 0;
}

