#include <iostream>
#include <unistd.h>
#include "aymmap/global.hpp"
#include "aymmap/aymmap.h"


//using namespace iin;

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

int main() {
    using namespace aymmap;
    using traits_type = FileMap::traits_type;

    auto ph = fs::path("test.txt");
    auto flag = AccessFlag::kDefault;

    FileMap fmap;
    std::cout << "map: " << fmap.map(ph, flag, 1024) << std::endl;
    std::cout << fmap.size() << std::endl;
    char * s = (char*)fmap.m_data.p_data_;
    std::string text = "1234";
    std::copy_n(text.c_str(), text.size(), s);
    s[0] = '0';
    std::cout << "flush: " << fmap.flush() << std::endl;
    std::cout << "unmap: " << fmap.unmap() << std::endl;

    FileUtils::removeFile(ph);

    return 0;
}

#if 0
int main()
{
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
#endif

