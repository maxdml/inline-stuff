#ifndef MSR_H_
#define MSR_H_

#include <mutex>
#include <memory>
#include "types.h"

class MsrHandle {
    int32_t fd;
    uint32_t cpu_id;
    MsrHandle();                                // forbidden
    MsrHandle(const MsrHandle &);               // forbidden
    MsrHandle & operator = (const MsrHandle &); // forbidden

public:
    MsrHandle(uint32_t cpu);
    int32_t read(uint64_t msr_number, uint64_t * value);
    int32_t write(uint64_t msr_number, uint64_t value);
    int32_t getCoreId() { return (int32_t)cpu_id; }
    virtual ~MsrHandle();
};

class SafeMsrHandle {
    std::shared_ptr<MsrHandle> pHandle;
    std::mutex mutex;

    SafeMsrHandle(const SafeMsrHandle &);               // forbidden
    SafeMsrHandle & operator = (const SafeMsrHandle &); // forbidden

public:
    SafeMsrHandle() { }

    SafeMsrHandle(uint32_t core_id) : pHandle(new MsrHandle(core_id))
    { }

    int32_t read(uint64_t msr_number, uint64_t * value) {
        if (pHandle)
            return pHandle->read(msr_number, value);

        *value = 0;

        return (int32_t)sizeof(uint64_t);
    }

    int32_t write(uint64_t msr_number, uint64_t value) {
        if (pHandle)
            return pHandle->write(msr_number, value);

        return (int32_t)sizeof(uint64_t);
    }

    int32_t getCoreId() {
        if (pHandle)
            return pHandle->getCoreId();

        throw std::exception();
        return -1;
    }

    void lock() {
        mutex.lock();
    }

    void unlock() {
        mutex.unlock();
    }

    virtual ~SafeMsrHandle()
    { }
};

#endif // MSR_H_
