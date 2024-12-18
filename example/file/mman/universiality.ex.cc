#include <iostream>

#include "aymmap/aymmap.hpp"

using namespace aymmap;

std::string errMsg() {
    return std::error_code{
        MemMapTraits::lastErrno(), std::system_category()
    }.message();
}

void doFileMap(MemMapTraits::handle_type handle) {
    size_t f_size = 10;
    if (!MemMapTraits::fileResize(handle, f_size)) {
        std::cout << "File resize failed: " << errMsg() << std::endl;
        return;
    }

    auto len = MemMapTraits::fileSize(handle);
    assert(len == f_size);

    MemMapData d;
    d.file_handle_ = handle;

    if (!MemMapTraits::map(d, AccessFlag::kDefault, len, 0)) {
        std::cout << "Map failed: " << errMsg() << std::endl;
        return;
    }

    char * mmapped = (char *)d.p_data_;
    std::string text = "1234567890";
    std::copy_n(text.c_str(), text.size(), mmapped);

    if (!MemMapTraits::sync(d.p_data_, len)) {
        std::cout << "Sync failed: " << errMsg() << std::endl;
    }

    f_size = 5;
    if (!MemMapTraits::remap(d, f_size)) {
        std::cout << "Remap failed: " << errMsg() << std::endl;
    } else {
        auto new_len = MemMapTraits::fileSize(handle);
        assert(new_len == f_size);
    }

    if (!MemMapTraits::unmap(d)) {
        std::cout << "Unmap failed: " << errMsg() << std::endl;
    }
}

int main() {
    std::cout << "Page size: " << MemMapTraits::pageSize() << std::endl;

    auto ph = fs::path("test.txt");

    auto handle = MemMapTraits::fileOpen(ph, AccessFlag::kDefault);
    if (handle == kInvalidHandle) {
        std::cout << "File open failed: " << errMsg() << std::endl;
        return -1;
    }
    
    doFileMap(handle);

    MemMapTraits::fileClose(handle);

    fs::remove(ph);
    return 0;
}

