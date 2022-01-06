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
int doneInt = 0;

int ordernum = 1;
int use_ptr  = 0;
int fill_ptr = 0;
int num_full = 0;

Cond invalid;
Cond odd;
Cond even;
Cond done;
Cond empty;
Cond fill;
Mutex m;
Mutex f;

int consumers = 4;


void do_fill(int value) {
    if (value != -2) { 
        buffer[fill_ptr] = value;
        fill_ptr = (fill_ptr + 1) % max;
        num_full++;
    }
    if (value == -2) {
        buffer[fill_ptr] = value;
        num_full++;
    }
}

int do_get_fizz() {
    int tmp = buffer[use_ptr];
    if (tmp == -2) {
      //  use_ptr = (use_ptr + 1) % max;
            //num_full =1;
            return tmp;
    }     
    if (tmp % 15 != 0 && tmp % 5 != 0) {
        if (tmp % 3 == 0) {
            use_ptr = (use_ptr + 1) % max;
            num_full--;
            return tmp;
            } 
        }
    return -1;
}

int do_get_buzz() {
    int tmp = buffer[use_ptr];
    if (tmp == -2) {
           //use_ptr = (use_ptr + 1) % max;
            //num_full = 1;
            return tmp;
    }     
    if (tmp % 15 != 0 && tmp % 3 != 0) {
        if (tmp % 5 == 0) {
            use_ptr = (use_ptr + 1) % max;
            num_full--;
            return tmp;
            } 
    }
    return -1;
}
int do_get_fizzBuzz() {
    int tmp = buffer[use_ptr];
    if (tmp == -2) {
            //use_ptr = (use_ptr + 1) % max;
            //num_full = 1;
            return tmp;
        }     
    if (tmp % 15 == 0) {
            use_ptr = (use_ptr + 1) % max;
            num_full--;
            return tmp;
        } 
        return -1;
}

int do_get() {
    int tmp = buffer[use_ptr];
    if (tmp == -2) {
      //  use_ptr = (use_ptr + 1) % max;
            //num_full = 1;
            return tmp;
    }     
    if (tmp % 15 != 0 && tmp % 3 != 0 && tmp % 5 != 0) {
            use_ptr = (use_ptr + 1) % max;
            num_full--;
            return tmp;
        } 
    return -1;    
}

void *producer_odd(void *) {
    int i;
    for (i = 1; i <= loops; i+=2) {
       
            m.lock();   
            while (ordernum == 0) {
                odd.wait(m);
            } 
            while (num_full == max) {   
                empty.wait(m);
            }
          
            ordernum--;
         //   std::cout << i << std::endl;
            do_fill(i); 
                              
            fill.signal(); 
            even.signal();         
            m.unlock();                
            
    }
     /*   f.lock();
        if (doneInt == 0){
            doneInt++;
        } 
        done.signal();
       */ //f.unlock();
        if (loops % 2 != 0){
        m.lock();
        while(num_full == max) {
            empty.wait(m);
        }
        do_fill(-2);
        fill.signal();
        m.unlock();
        }
    invalid.broadcast();
    fill.broadcast();
    return NULL;
}
void *producer_even(void *) {
    int i;
    for (i = 0; i <= loops; i+=2) {
            m.lock();
            while (ordernum == 1) {
                even.wait(m);
            }
            while (num_full == max) {
                empty.wait(m);
            }
            
            ordernum++;
            do_fill(i);  
              
            fill.signal();    
            odd.signal();
            m.unlock();  
                          
    }


      /*  f.lock();
        while (doneInt != 1) {
            done.wait(f);
        } 
        f.unlock();
        */
       if (loops % 2 == 0){
        m.lock();
        while(num_full == max) {
            empty.wait(m);
        }
        do_fill(-2);
        fill.signal();
        m.unlock();
       }
    invalid.broadcast();
    fill.broadcast();
    return NULL;
}

void *consumer_buzz(void *) {
    int tmp = 0;  
     
    while (tmp != -2) { 
        m.lock();
        //std::cout << "buzz lock aqcuired" << std::endl;
        while (num_full == 0) {    
            fill.wait(m);
            //std::cout << "buzz wait empty" << std::endl;
        }         
        tmp = do_get_buzz();  
            //std::cout << "buzz calls get " << tmp << std::endl; 
        if (tmp == -1) {
            //std::cout << "buzz invalid.signal 1" << std::endl;
            invalid.signal();
            //std::cout << "buzz wait invalid" << std::endl;
            invalid.wait(m);
            //std::cout << "buzz invalid.siignal 2" << std::endl;
            invalid.signal();
        } else if (tmp > 0) {
            std::cout << "Buzz" << std::endl;
            //std::cout << "buzz signal empty after consumption" << std::endl;
            empty.signal();
            //std::cout << "buzz signal empty after consumption" << std::endl;
            invalid.signal();
        }
        //std::cout << tmp << " at unlockc  buzz" << std::endl;
        m.unlock();   
    }
    invalid.broadcast();
    fill.broadcast();
    //std::cout << "BUZZ END FXN" << std::endl;
    return NULL;
}

void *consumer_fizz(void *) {
    int tmp = 0;        
     
     while (tmp != -2) { 
        m.lock();
        //std::cout << "fizz acquires lock" << std::endl;
        while (num_full == 0) {    
            fill.wait(m);
            //std::cout << "f wait empty" << std::endl;        
        }
        tmp = do_get_fizz(); 
        //std::cout << "fizz calls get fizz " << tmp << std::endl;
        if (tmp == -1) {
            //std::cout << "fizz signal invalid 1" << std::endl;
            invalid.signal();
            //std::cout << "fizz wait invalid" << std::endl;
            invalid.wait(m);
            //std::cout << "fizz signal invalid 2" << std::endl;
            invalid.signal();

        }else if (tmp > 0) {
        std::cout << "Fizz" << std::endl;
        //std::cout << "fizz signal empty post consumption" << std::endl;
        empty.signal();
        //std::cout << "fizz signal invalid post consumption" << std::endl;
        invalid.signal();            

        }
        //std::cout << tmp << " tmp before unlock" << std::endl;
        m.unlock();              
    }
    //std::cout << "fizz signal invalid post unlock" << std::endl;
    invalid.broadcast();
    //std::cout << "fizz DONE" << std::endl;
    fill.broadcast();
    return NULL;
}

void *consumer_fizzBuzz(void *) {
      int tmp = 0;  
  
     while (tmp != -2) { 
        m.lock();
        //std::cout << "fizzbuzz acquires lock" << std::endl;
        while (num_full == 0) {     
            fill.wait(m);    
            //std::cout << "fb wait empty" << std::endl;
        }     
        tmp = do_get_fizzBuzz(); 
        //std::cout << "fizzbuzz tries to get number" << std::endl;
        if (tmp == -1) {
            //std::cout << "fizzbuzz invalid signal 1" << std::endl;
            invalid.signal();
            //std::cout << "fizzbuzz invalid wait" << std::endl;
            invalid.wait(m);
            //std::cout << "fizzbuzz invalid signal 2" << std::endl;
            invalid.signal();
        //    std::cout << "fb wait -1" << std::endl;
        } else if (tmp > 0) {    
            std::cout << "FizzBuzz" << std::endl;
            empty.signal();  
            //std::cout << "fizzbuzz empty signal post consumption" << std::endl;
            invalid.signal();
            //std::cout << "fizzbuzz invalid signal post consumption" << std::endl;
        }
        //std::cout << tmp << " tmp value at unlock" << std::endl;   
        m.unlock();    
    } 
    //std::cout << "fizzbuzz invalid signal post lock" << std::endl;
    invalid.broadcast();
    //std::cout << "fizzbuzz fill signal post lock" << std::endl;
    fill.broadcast();
    return NULL;
}

void *consumer_num(void *) {
    int tmp = 0;
  
     while (tmp != -2) { 
        m.lock();
        //std::cout << "num acquires lock" << std::endl;
        while (num_full == 0) {   
            fill.wait(m);      
            //std::cout << "n wait empty" << std::endl;
        }  
        tmp = do_get();
        //std::cout << "num attempts to consume" << std::endl;
        if(tmp == -1) {
            //std::cout << "num invalid signal 1" << std::endl;
            invalid.signal();
            //std::cout << "num invalid wait" << std::endl;
            invalid.wait(m);
            //std::cout << "num invalid signal 2" << std::endl;
            invalid.signal();
        } else if (tmp > 0) {   
        std::cout << tmp << std::endl;
        //std::cout << "num invalid signal post consumption" << std::endl;
        invalid.signal();
        //std::cout << "num empty signal post consumption" << std::endl;
        empty.signal();

        }
        //std::cout << tmp <<  " right before unlock" << std::endl;
        m.unlock();         
    } 
    //std::cout << "num invalid signal post loop" << std::endl;
    invalid.broadcast();
    //std::cout << "num fill signal post loop" << std::endl;
    fill.broadcast();

    return NULL;
}


int main(int argc, char *argv[])
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

    pid.create(producer_odd);
    pid2.create(producer_even); 

    vector<Thread> cid(consumers);
    cid[0].create(consumer_fizz, (void *) (long long int) 0);
    cid[1].create(consumer_buzz, (void *) (long long int) 1);
    cid[2].create(consumer_fizzBuzz, (void *) (long long int) 2);
    cid[3].create(consumer_num, (void *) (long long int) 3); 
    
    pid.join();
    std::cout << "p1 closed" << std::endl;
    pid2.join(); 
    std::cout << "p2 closed" << std::endl;
    for (i = 0; i < consumers; i++) {
    cid[i].join(); 
    std::cout << i << " is closed " << std::endl;
    }
    return 0;
}
    