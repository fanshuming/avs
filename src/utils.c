#include <semaphore.h>
#include "utils.h"
#include "log.h"

void sem_wakeup_init(void)
{
        sem_init(&sem_mic_wakeup, 0, 0);
}

void post_wakeup(void)
{
        LOGD("[SEMAPHORE]:post wake up\n");
        sem_post(&sem_mic_wakeup);
}

/* mic wakeup */
//ret = sem_wait(&sem_mic_wakeup);

