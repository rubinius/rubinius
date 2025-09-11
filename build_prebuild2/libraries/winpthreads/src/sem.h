#ifndef WIN_SEM
#define WIN_SEM

#include <windows.h>
#include "mutex.h"

#define LIFE_SEM 0xBAB1F00D
#define DEAD_SEM 0xDEADBEEF

typedef struct _sem_t _sem_t;
struct _sem_t
{
    unsigned int valid;
    HANDLE s;
    volatile int value;
    pthread_mutex_t vlock;
};

#endif /* WIN_SEM */