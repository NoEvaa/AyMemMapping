#include <iostream>
#include <system_error>
#include "aymmap/global.hpp"
#include "aymmap/aymmap.h"
#include "aymmap/mmap_file.hpp"

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

template <>
class aymmap::MMapFileFriend<int> {
public:
    void call(aymmap::MMapFile & fi) {
        std::cout << fi.m_length << std::endl;
    }
};

int case1() {
    using namespace aymmap;

    auto ph = fs::path("你好.txt");
    auto flag = AccessFlag::kDefault;
    auto fd = MemMapTraits::fileOpen(ph, flag);
    if (fd == kInvalidHandle) {
        std::cout << "file open failed" << std::endl;
        return -1;
    }
    
    std::size_t f_size = 10;
    MemMapTraits::fileResize(fd, f_size);
    auto len = MemMapTraits::fileSize(fd);

    MemMapData d;
    d.file_handle_ = fd;

    if (!MemMapTraits::map(d, flag, len, 0)) {
        std::cout << "mmap failed" << std::endl;
        MemMapTraits::fileClose(fd);
        return -1;
    }

    char * mmapped = (char *)d.p_data_;
    std::string text = "哇34567890";
    std::copy_n(text.c_str(), text.size(), mmapped);

    if (!MemMapTraits::sync(d.p_data_, len)) {
        std::cout << "msync failed" << std::endl;
    }

    if (!MemMapTraits::remap(d, 5)) {
        std::cout << "remap failed" << std::endl;
    }

    MemMapTraits::unmap(d);
    MemMapTraits::fileClose(fd);
    MemMapTraits::fileRemove(ph);
    return 0;
}

int case2() {
    using namespace aymmap;
    using traits_type = MMapFile::traits_type;

    auto ph = fs::path("./build/test.txt");
    auto flag = AccessFlag::kDefault;

    MMapFile fmap;
    if (auto en = fmap.map(ph, flag, 1024)) {
        std::cout << "map: " << en << " "
            << std::error_code{en, std::system_category()}
            << std::endl;
    }

    MMapFileFriend<int>().call(fmap);
    std::cout << "size: " << fmap.size() << std::endl;

    char * s = fmap.data();
    std::string text = "1234";
    std::copy_n(text.c_str(), text.size(), s);
    s[0] = '0';

    std::cout << "flush: " << fmap.flush() << std::endl;
    std::cout << "unmap: " << fmap.unmap() << std::endl;
    MMapFile::removeFile(ph);
    return 0;
}

int main() {
    return case2();
}

