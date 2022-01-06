#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include "pthread-wrappers.h"


void Thread::create(void *(*routine)(void *), void *arg)
{
    errno = pthread_create(&tid, NULL, routine, arg);
    if (errno != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
}

void *Thread::join()
{
    void *retval;
    errno = pthread_join(tid, &retval);
    if (errno != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    return retval;
}


Mutex::Mutex()
{
    errno = pthread_mutex_init(&mutex, NULL);
    if (errno != 0) {
        perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
    }
}

Mutex::~Mutex()
{
    errno = pthread_mutex_destroy(&mutex);
    if (errno != 0) {
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }
}

void Mutex::lock()
{
    errno = pthread_mutex_lock(&mutex);
    if (errno != 0) {
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
}

void Mutex::unlock()
{
    errno = pthread_mutex_unlock(&mutex);
    if (errno != 0) {
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}

Cond::Cond()
{
    errno = pthread_cond_init(&cond, NULL);
    if (errno != 0) {
        perror("pthread_cond_init");
        exit(EXIT_FAILURE);
    }
}

Cond::~Cond()
{
    errno = pthread_cond_destroy(&cond);
    if (errno != 0) {
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);
    }
}

void Cond::wait(Mutex &m)
{
    errno = pthread_cond_wait(&cond, &m.mutex);
    if (errno != 0) {
        perror("pthread_cond_wait");
        exit(EXIT_FAILURE);
    }
}

void Cond::broadcast()
{
    errno = pthread_cond_broadcast(&cond);

    if (errno != 0) {
        perror("pthread_cond_broadcast");
        exit(EXIT_FAILURE);
    }
}

void Cond::signal()
{
    errno = pthread_cond_signal(&cond);
    if (errno != 0) {
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }
}
