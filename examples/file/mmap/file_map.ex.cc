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

#include <iostream>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

std::string errMsg(errno_t en) {
    return std::error_code{en, std::system_category()}.message();
}

void printMap(MMapFile & mmfi) {
    std::cout << "Map size: " << mmfi.size() << std::endl;

    std::cout << "Content: ";
    for (auto iter = mmfi.begin(); iter != mmfi.end(); ++iter) {
        std::cout << *iter;
    }
    std::cout << std::endl;
}

int createAndWriteFile() {
    std::cout << "- Write file..." << std::endl;

    auto * fi = ::fopen("test.txt", "wb+");
    if (!fi) { return -1; }

    MMapFile mmfi;
    if (auto en = mmfi.fileMap(fi, AccessFlag::kDefault, true, 10)) {
        std::cout << "File map failed: [" << en << "] "
            << errMsg(en) << std::endl;
        ::fclose(fi);
        return -1;
    }
    // file handle is duplicated, so we can close the file here
    ::fclose(fi);
    assert(mmfi.isMapped());
    assert(!mmfi.isAnon());

    char * s = mmfi.data();
    std::string text = "1234567890";
    std::copy_n(text.c_str(), text.size(), s);
    s[0] = '0';

    mmfi.flush();
    mmfi.unmap();
    return 0;
}

int readFile() {
    std::cout << "- Read file..." << std::endl;

    auto * fi = ::fopen("test.txt", "rb");
    if (!fi) { return -1; }

    MMapFile mmfi;
    if (auto en = mmfi.fileMap(fi, AccessFlag::kReadOnly)) {
        std::cout << "File map failed: [" << en << "] "
            << errMsg(en) << std::endl;
        ::fclose(fi);
        return -1;
    }
    ::fclose(fi);
    assert(mmfi.isMapped());

    printMap(mmfi);

    mmfi.unmap();
    return 0;
}

int modifyFile() {
    std::cout << "- Modify file..." << std::endl;

    auto * fi = ::fopen("test.txt", "rb+");
    if (!fi) { return -1; }

    MMapFile mmfi;
    bool b_dup_handle = false;
    // resize the file when open
    if (auto en = mmfi.fileMap(fi,
        AccessFlag::kReadWrite | AccessFlag::kResize,
        b_dup_handle, 5)) {
        std::cout << "File map failed: [" << en << "] "
            << errMsg(en) << std::endl;
        ::fclose(fi);
        return -1;
    }
    assert(mmfi.isMapped());

    printMap(mmfi);

    mmfi.unmap();
    ::fclose(fi);
    return 0;
}

int main() {
    createAndWriteFile();
    readFile();
    modifyFile();
    fs::remove("test.txt");
    return 0;
}

