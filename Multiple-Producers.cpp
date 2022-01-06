#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <iostream>

#include <unistd.h>

#include "pthread-wrappers.h"

using std::vector;

int max;
int loops;
int *buffer; 

int use_ptr  = 0;
int fill_ptr = 0;
int num_full = 0;

Cond empty;
Cond fill;
Mutex m;

int consumers = 1;
int verbose = 1;


void do_fill(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % max;
    num_full++;
}

int do_get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % max;
    num_full--;
    return tmp;
}

void *producer_odd(void *) {
    int i;
    for (i = 1; i <= loops; i+=2) {
       
            m.lock();                  // p1
            while (num_full == max)    // p2
                empty.wait(m);         // p3
            do_fill(i);                // p4
            fill.signal();             // p5
            m.unlock();                // p6

    }
    return NULL;
}
void *producer_even(void *) {
    int i;
    for (i = 0; i <= loops; i+=2) {
            m.lock();                  // p1
            while (num_full == max)    // p2
                empty.wait(m);         // p3
            do_fill(i);                // p4
            fill.signal();             // p5
            m.unlock();                // p6
    }
    return NULL;
}

void *consumer(void *) {

      int i;
      for (i = 0; i <= loops; i++) { 
        m.lock();
        while (num_full == 0)     // c2 
            fill.wait(m);         // c3
        int tmp = do_get(); 
    
            if (tmp % 15 == 0) {
                std::cout << "FizzBuzz" << std::endl;
            } else if (tmp % 5 == 0) {
                std::cout << "Buzz" << std::endl;
            } else if (tmp % 3 == 0) {
                std::cout << "Fizz" << std::endl;
            } else {
                std::cout << tmp << std::endl;
            }
              // c4
        empty.signal();           // c5
        m.unlock();               // c6
        }

    return NULL;
}



int
main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage ./fizzbuzz [number] [buffersize] " << std::endl;
        return 1;
    }
    loops = atoi(argv[1]);
    max = atoi(argv[2]);

    buffer = (int *) malloc(max * sizeof(int));
    assert(buffer != NULL);

    int i;
    for (i = 0; i < max; i++) {
    buffer[i] = 0;
    }

    Thread pid;
    Thread pid2;
    vector<Thread> cid(consumers);
    pid.create(producer_odd);
    pid2.create(producer_even); 
    for (i = 0; i < consumers; i++) {
    cid[i].create(consumer, (void *) (long long int) i); 
    }
    pid.join();
    pid2.join(); 
    for (i = 0; i < consumers; i++) {
    cid[i].join(); 
    }
    return 0;
}

    
