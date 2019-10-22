#include <stdio.h>
#include <iostream>
#include <stdint.h>

void msr(uint64_t addr, uint32_t* eax, uint32_t* edx) 
{
    //asm volatile
    //  ("rdmsr" 
    //   : "=a" (*eax), "=d" (*edx) 
    //   : "c" (addr)
    //  );
}


MsrHandle::MsrHandle(uint32 cpu) : cpu_id(cpu)
{
    hDriver = CreateFile(L"\\\\.\\RDMSR", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hDriver == INVALID_HANDLE_VALUE && hOpenLibSys == NULL)
        throw std::exception();
}

MsrHandle::~MsrHandle()
{
    if (hDriver != INVALID_HANDLE_VALUE) CloseHandle(hDriver);
}

int32 MsrHandle::write(uint64 msr_number, uint64 value)
{
    if (hDriver != INVALID_HANDLE_VALUE)
    {
        MSR_Request req;
        ULONG64 result;
        DWORD reslength = 0;
        req.core_id = cpu_id;
        req.msr_address = msr_number;
        req.write_value = value;
        BOOL status = DeviceIoControl(hDriver, IO_CTL_MSR_WRITE, &req, sizeof(MSR_Request), &result, sizeof(uint64), &reslength, NULL);
        assert(status && "Error in DeviceIoControl");
        return reslength;
    }

    cvt_ds cvt;
    cvt.ui64 = value;

    ThreadGroupTempAffinity affinity(cpu_id);
    DWORD status = Wrmsr((DWORD)msr_number, cvt.ui32.low, cvt.ui32.high);

    return status ? sizeof(uint64) : 0;
}

int32 MsrHandle::read(uint64 msr_number, uint64 * value)
{
    if (hDriver != INVALID_HANDLE_VALUE)
    {
        MSR_Request req;
        // ULONG64 result;
        DWORD reslength = 0;
        req.core_id = cpu_id;
        req.msr_address = msr_number;
        BOOL status = DeviceIoControl(hDriver, IO_CTL_MSR_READ, &req, sizeof(MSR_Request), value, sizeof(uint64), &reslength, NULL);
        assert(status && "Error in DeviceIoControl");
        return (int32)reslength;
    }

    cvt_ds cvt;
    cvt.ui64 = 0;

    ThreadGroupTempAffinity affinity(cpu_id);
    DWORD status = Rdmsr((DWORD)msr_number, &(cvt.ui32.low), &(cvt.ui32.high));

    if (status) *value = cvt.ui64;

    return status ? sizeof(uint64) : 0;
}

