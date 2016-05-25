#ifndef PHOSAL_POSIX_THREAD_H_
#define PHOSAL_POSIX_THREAD_H_

#include <ph_Status.h>
#include <phOsal.h>


extern phStatus_t phOsal_Posix_Thread_Create(phOsal_EventDestination_t threadName, void *(*start_routine) (void *), void *arg);

extern void phOsal_Posix_Thread_Join(phOsal_EventDestination_t t, void** returnCode);

extern phStatus_t phOsal_Posix_Thread_Create_Extra(pthread_t *thread, void *(*start_routine) (void *),
        void *arg);

extern void phOsal_Posix_Thread_Join_Extra(pthread_t *thread, void** returnCode);


#endif /* PHOSAL_POSIX_THREAD_H_ */
