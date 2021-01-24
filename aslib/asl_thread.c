#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_memory.h"
#include "asl_thread.h"

result_t asl_sem_create(struct asl_sem *sem, char* name, s_int32_t initCount)
{
    int len = 0;
    if(name != NULL)
    {
        len = strlen(name) + 1; 
    }
    if(sem != NULL && sem_init(&(sem->sem), 0, initCount) == 0)
    {
        if(len != 0)
        {
            sem->name = (char*)asl_malloc(len);
            strncpy(sem->name, name, len - 1);
        }
        else
        {
            len = strlen(SEM_DEFAULT_NAME) + 1;
            sem->name = (char*)asl_malloc(len);
            strncpy(sem->name, SEM_DEFAULT_NAME, len);
        }
        
        sem->count = initCount;
        sem->state = SEM_INITIALIED;
        return SUCCESS;
    }
    if(sem != NULL)
    {
        asl_print_err("Create \"%s\" sem FAILED", name == NULL ? SEM_DEFAULT_NAME : name);
    }
    return FAILURE;
}
result_t asl_sem_destroy(struct asl_sem *sem)
{
    if(sem != NULL && sem_destroy(&(sem->sem)) < 0)
    {
        asl_print_dbg("Destroy \"%s\" sem FAILED", sem->name);
        return FAILURE;
    }
    asl_free(sem->name);
    sem->count = 0;
    sem->state = !SEM_INITIALIED;
    return SUCCESS;
}
result_t asl_sem_wait(struct asl_sem *sem)
{
    if(sem == NULL)
    {
        asl_print_err("Sem is NULL");
        return FAILURE;
    }
    if(sem_wait(&(sem->sem)) < 0)
    {
        //asl_print_dbg("Wait \"%s\" sem FAILED", sem->name);
        return FAILURE;
    }
    //asl_print_dbg("Wait \"%s\" sem SUCCESS", sem->name);
    sem->count--;
    return SUCCESS;
}
result_t asl_sem_trywait(struct asl_sem *sem)
{
    if(sem == NULL)
    {
        asl_print_err("Sem is NULL");
        return FAILURE;
    }
    if(sem_trywait(&(sem->sem)) < 0)
    {
        //asl_print_dbg("Wait \"%s\" sem FAILED", sem->name);
        return FAILURE;
    }
    //asl_print_dbg("Wait \"%s\" sem SUCCESS", sem->name);
    sem->count--;
    return SUCCESS;
}

result_t asl_sem_post(struct asl_sem *sem)
{
    if(sem == NULL)
    {
        asl_print_err("Sem is NULL");
        return FAILURE;
    }
    if(sem_post(&(sem->sem)) < 0)
    {
        //asl_print_dbg("Trywait \"%s\" sem FAILED", sem->name);
        return FAILURE;
    }
    //asl_print_dbg("Trywait \"%s\" sem SUCCESS", sem->name);
    sem->count++;
    return SUCCESS;
}

result_t asl_thread_create(struct asl_thread *thread, void* func, void* param)
{
    if(thread != NULL && (pthread_create(&(thread->id), NULL, (void*(*)(void*))func, param) == 0))
    {
        /* Ensure the thread's resource is released when the thread is terminated */
        //pthread_detach(thread->id);
        return SUCCESS;
    }
    return FAILURE;
}
result_t asl_thread_detach(struct asl_thread *thread)
{
    if(thread != NULL)
    {
        pthread_detach(thread->id);
    }
    return SUCCESS;
}
result_t asl_thread_join(struct asl_thread *thread)
{
    return pthread_join(thread->id, NULL);
}

result_t asl_thread_cancel(struct asl_thread *thread)
{
    return pthread_cancel(thread->id);
}

result_t asl_thread_mutex_init(struct asl_thread_mutex *m)
{
    if(pthread_mutex_init(&m->m, NULL) == 0)
        return SUCCESS;
    return FAILURE;
}
result_t asl_thread_mutex_lock(struct asl_thread_mutex *m)
{
    if(pthread_mutex_lock(&m->m) == 0)
        return SUCCESS;
    return FAILURE;
}
result_t asl_thread_mutex_unlock(struct asl_thread_mutex *m)
{
    if(pthread_mutex_unlock(&m->m) == 0)
        return SUCCESS;
    return FAILURE;
}
result_t asl_thread_mutex_trylock(struct asl_thread_mutex *m)
{
    if(pthread_mutex_trylock(&m->m) == 0)
        return SUCCESS;
    return FAILURE;
}
result_t asl_thread_mutex_destroy(struct  asl_thread_mutex *m)
{
    if(pthread_mutex_destroy(&m->m) == 0)
        return SUCCESS;
    return FAILURE;
}

