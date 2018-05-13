#include <semaphore.h>

sem_t sem_mic_wakeup;

void sem_wakeup_init(void);

void post_wakeup(void);

