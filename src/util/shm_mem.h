#ifndef _SHM_MEM_H_
#define _SHM_MEM_H_

#include <sys/shm.h>

namespace util
{
    void* ShmInit(key_t key, size_t size, bool &exist);
    bool ShmDelete(key_t key);
};

#endif
