#include <asm-generic/fcntl.h>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include "aymmap/detail/mman_unix.tcc"
#include "aymmap/global.hpp"
#include "aymmap/mman.hpp"

//using namespace iin;

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

int main()
{
    using namespace aymmap;
    auto ph = fs::path("你好.txt");
    AccessFlag flag = AccessFlag::kDefault;
    auto fd = MemMapTraits::openFile(ph, flag);
    if (fd == -1)
        return -1;
    
    std::size_t f_size = 1024;
    MemMapTraits::resizeFile(fd, f_size);
    auto len = MemMapTraits::fileSize(fd);

    MemMapData d;
    d.handle_ = fd;

    if (!MemMapTraits::map(d, flag, len, 0)) {
        std::cout << "mmap failed" << std::endl;
        MemMapTraits::closeFile(fd);
        return -1;
    }

    char * mmapped = (char *)d.p_data_;
    mmapped[0] = 'A';

    if (!MemMapTraits::sync(d.p_data_, len)) {
        std::cout << "msync failed" << std::endl;
    }

    MemMapTraits::unmap(d);
    MemMapTraits::closeFile(fd);
    MemMapTraits::removeFile(ph);
    return 0;
}

