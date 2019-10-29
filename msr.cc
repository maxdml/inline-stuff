#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include  <iostream>

#include "msr.hh"

MsrHandle::MsrHandle(uint32_t cpu) : fd(-1), cpu_id(cpu) {
    char * path = new char[200];
    snprintf(path, 200, "/dev/cpu/%d/msr", cpu);
    int handle = ::open(path, O_RDWR);
    delete[] path;
    if (handle < 0) {
         std::cerr << "PCM Error: can't open MSR handle for core " << cpu << std::endl;
         throw std::exception();
    }
    fd = handle;
}

MsrHandle::~MsrHandle() {
    if (fd >= 0) ::close(fd);
}

int32_t MsrHandle::write(uint64_t msr_number, uint64_t value) {
    return ::pwrite(fd, (const void *)&value, sizeof(uint64_t), msr_number);
}

int32_t MsrHandle::read(uint64_t msr_number, uint64_t * value) {
    return ::pread(fd, (void *)value, sizeof(uint64_t), msr_number);
}
