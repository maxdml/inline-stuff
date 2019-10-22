#include <sys/ioccom.h>
#include <sys/cpuctl.h>

MsrHandle::MsrHandle(uint32 cpu) : fd(-1), cpu_id(cpu)
{
    char path[200];
    snprintf(path, 200, "/dev/cpuctl%d", cpu);
    int handle = ::open(path, O_RDWR);
    if (handle < 0) throw std::exception();
    fd = handle;

}

MsrHandle::~MsrHandle()
{
    if (fd >= 0) ::close(fd);
}

int32 MsrHandle::write(uint64 msr_number, uint64 value)
{
    cpuctl_msr_args_t args;
    int ret;

    args.msr = msr_number;
    args.data = value;
    ret = ::ioctl(fd, CPUCTL_WRMSR, &args);
    if (ret) return ret;
    return sizeof(value);
}

int32 MsrHandle::read(uint64 msr_number, uint64 * value)
{
    cpuctl_msr_args_t args;
    int32 ret;

    args.msr = msr_number;
    ret = ::ioctl(fd, CPUCTL_RDMSR, &args);
    if (ret) return ret;
    *value = args.data;
    return sizeof(*value);

}

