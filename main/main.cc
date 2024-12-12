#include <cstdint>
#include <iostream>
#include <system_error>
#include <unistd.h>
#include <locale>
#include "aymmap/global.hpp"
#include "aymmap/mman.hpp"


//using namespace iin;

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

int main()
{
    std::locale::global(std::locale("en_US.UTF-8"));

    using namespace aymmap;

    /*
    auto ph1 = fs::path("111.txt");
    auto fd1 = MemMapTraits::fileOpen(ph1, AccessFlag::kWrite);
    if (fd1 == -1) {
        std::cout << std::error_code(-1, std::system_category()).message() << std::endl;
        std::cout << errno << std::endl;
        std::cout << "file open failed" << std::endl;
        return -1;
    }
    */

    auto ph = fs::path(u8"你好.txt");
    AccessFlag flag = AccessFlag::kDefault;
    auto fd = MemMapTraits::fileOpen(ph, flag);
    if (fd == -1) {
        std::cout << "file open failed" << std::endl;
        return -1;
    }
    
    std::size_t f_size = 1024;
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
    mmapped[0] = 'A';

    if (!MemMapTraits::sync(d.p_data_, len)) {
        std::cout << "msync failed" << std::endl;
    }

    MemMapTraits::unmap(d);
    MemMapTraits::fileClose(fd);
    MemMapTraits::fileRemove(ph);
    return 0;
}

