#ifndef ASL_THREAD_H
#define ASL_THREAD_H

#include <semaphore.h>
#include <pthread.h>

typedef void*(*asl_thr_func)(void*);

#define SEM_INITIALIED (TRUE)
#define SEM_DEFAULT_NAME "UNKNOWN"
struct asl_sem
{
    char* name;
    sem_t sem;
    s_int32_t count;
    bool state;
};

result_t asl_sem_create(struct asl_sem *sem, char* name, bool initState);
result_t asl_sem_destroy(struct asl_sem *sem);
result_t asl_sem_wait(struct asl_sem *sem);
result_t asl_sem_trywait(struct asl_sem *sem);
result_t asl_sem_post(struct asl_sem *sem);

struct asl_thread
{
    pthread_t id;
    void* func;
    void* param;
};
result_t asl_thread_create(struct asl_thread *thread, void* func, void* param);
result_t asl_thread_detach(struct asl_thread *thread);
result_t asl_thread_join(struct asl_thread *thread);
result_t asl_thread_cancel(struct asl_thread *thread);
#define asl_thread_set_cancel_enable() pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)
#define asl_thread_set_cancel_disable() pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)
#define asl_thread_cleanup_begin(func, arg) pthread_cleanup_push(func, arg)
#define ASL_THREAD_NEED_CLEANUP 1
#define ASL_THREAD_NOT_NEED_CLEANUP 0
#define asl_thread_cleanup_end(val) pthread_cleanup_pop(val)
struct asl_thread_mutex
{
    pthread_mutex_t m;
};

result_t asl_thread_mutex_init(struct asl_thread_mutex *m);
result_t asl_thread_mutex_lock(struct asl_thread_mutex *m);
result_t asl_thread_mutex_unlock(struct asl_thread_mutex *m);
result_t asl_thread_mutex_trylock(struct asl_thread_mutex *m);
result_t asl_thread_mutex_destroy(struct  asl_thread_mutex *m);

#endif /* ASL_THREAD_H */
