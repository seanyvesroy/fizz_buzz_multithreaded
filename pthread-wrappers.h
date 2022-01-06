#ifndef PTHREAD_WRAPPERS_H
#define PTHREAD_WRAPPERS_H

#include <pthread.h>

class Thread {
    public:
        void create(void *(*thread)(void *), void * = NULL);
        void *join();
    private:
        pthread_t tid;
};

class Mutex {
    friend class Cond;

    public:
        Mutex();
        ~Mutex();
        void lock();
        void unlock();

    private:
        pthread_mutex_t mutex;
};

class Cond {
    public:
        Cond();
        ~Cond();
        void wait(Mutex &);
        void signal();
        void broadcast();

    private:
	pthread_cond_t cond;
};

#endif
