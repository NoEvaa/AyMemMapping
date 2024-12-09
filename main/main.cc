#include <asm-generic/fcntl.h>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include "aymmap/detail/mman_unix.hpp"

//using namespace iin;

// 4567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

int main()
{
    auto fd = ::open("你好.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1)
        return -1;
    
    off_t newSize = 1024; // 新的文件大小
if (ftruncate(fd, newSize) == -1) {
    close(fd);
    return -1;
}

struct stat sb;
if (fstat(fd, &sb) == -1) {
    ::close(fd);
    return -1;
}
char* mmapped = (char*)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (mmapped == MAP_FAILED) {
        std::cout << "mmap failed" << std::endl;
    ::close(fd);
    return -1;
}
    // 修改映射区域的内容
mmapped[0] = 'A';
if (msync(mmapped, sb.st_size, MS_SYNC) == -1) {
        std::cout << "msync failed" << std::endl;
}
if (munmap(mmapped, sb.st_size) == -1) {
}


    ::close(fd);
    return 0;
}

