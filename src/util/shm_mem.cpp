#include "shm_mem.h"
#include <errno.h> //used to get the last error code
#include <cstdio>
#include <cstring>
//#include "util.h"

void* util::ShmInit(key_t key, size_t size, bool& exist)
{
    exist = false;
    int shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0644); //0644 permission
    if(shm_id < 0)
    {
        //not file exists error
        if(errno != EEXIST)
        {
            return NULL;
        }

        exist = true;
        shm_id = shmget(key, size, 0644);
        if(shm_id < 0)
        {
            return NULL;
        }
    }

    void *p = shmat(shm_id, NULL, 0);
    if (!p)
    {
        return NULL;
    }

    //initialize
    if (!exist)
    {
        memset(p, 0, size);
    }

    return p;
}

bool util::ShmDelete(key_t key)
{
    int shm_id = shmget(key, 0, 0644);
    if(shm_id < 0)
    {
        return false;
    }

    int ret = shmctl(shm_id, IPC_RMID, NULL);

    return (ret != -1);
}
