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
Cond order;
Cond done;
Cond empty;
Cond fill;
Mutex m;
Mutex f;

int consumers = 4;


void do_fill(int value) {                      //fill function           
    if (value != -2) {                         //if the value isn't end of buffer marker add                          
        buffer[fill_ptr] = value;
        fill_ptr = (fill_ptr + 1) % max;
        num_full++;
    }
    if (value == -2) {                          //if it is end of buffer value stop incrementing 
        buffer[fill_ptr] = value;               //fill ptr so all threads end consuming this 
        num_full++;
    }
}
                                                //specialized get functions, one for each  consumer
int do_get_fizz() {     
    int tmp = buffer[use_ptr];
    if (tmp == -2) {                            //return end of buffer marker
            return tmp; 
    }     
    if (tmp % 15 != 0 && tmp % 5 != 0) {
        if (tmp % 3 == 0) {                     //check and return valid input          
            use_ptr = (use_ptr + 1) % max;
            num_full--;
            return tmp;
            } 
        }
    return -1;                                  //refers to invlid input for this fxn                            
}

int do_get_buzz() {
    int tmp = buffer[use_ptr];
    if (tmp == -2) {
          
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
    for (i = 1; i <= loops; i+=2) {     //loop for only odd numbers
       
            m.lock();                   //lock
            while (num_full == max) {   //if buffer empty wait
                empty.wait(m);
            }
            do_fill(i);                 //fill with value
                              
            fill.signal();              //signal
            m.unlock();                 //unlock
    
    }
        f.lock();                       //lock until both producer threads complete
        if (doneInt == 0){              //checks if other thread is done
            doneInt++;
        } 
        done.signal();                      
        f.unlock();

        invalid.broadcast();            //broadcast to wake all consumers who may be stuck waiting
        fill.broadcast();
    return NULL;
}
void *producer_even(void *) {
    int i;
    for (i = 0; i <= loops; i+=2) {
            m.lock();
            while (num_full == max) {
                empty.wait(m);
            }
          
            do_fill(i);  
              
            fill.signal();    
            m.unlock();  
                     
    }


        f.lock();                   //end of production
        while (doneInt != 1) {      
            done.wait(f);           //if not done we wait
        } 
        f.unlock();
        
        m.lock();
        while(num_full == max) {     //if buffer full wait
            empty.wait(m);
        }
        do_fill(-2);                 //place end of production marker   
        fill.signal();
        m.unlock();
  
    invalid.broadcast();                
    fill.broadcast();
    return NULL;
}

void *consumer_buzz(void *) {
    int tmp = 0;  
     
    while (tmp != -2) {                             //as long as value is not end of buffer marker, run
        m.lock();
       
        while (num_full == 0) {                     //if buffer empty, wait
            fill.wait(m);
        }         
        tmp = do_get_buzz();                        //call get function
      
        if (tmp == -1) {                            //if input is invalid signal other threads
            invalid.signal();                       //wait    
            invalid.wait(m);                        
            invalid.signal();                       //signal other threads that may be waiting on invalid input         
        } else if (tmp > 0) {                       //if input is valid
            
            std::cout << "Buzz" << std::endl;       //print
            empty.signal();                         //signal we consumed item
            invalid.signal();                       //wake possible sleeping threads on ivalid wait
        }
        m.unlock();   
    }
    invalid.broadcast();                            //once finished running wake all other still active consumers
    fill.broadcast();                               //that way no one is left behind
    return NULL;
}

void *consumer_fizz(void *) {
    int tmp = 0;        
     
     while (tmp != -2) { 
        m.lock();
        while (num_full == 0) {    
            fill.wait(m);
        }
        tmp = do_get_fizz();

        if (tmp == -1) {
            invalid.signal();
            invalid.wait(m);
            invalid.signal();

        }else if (tmp > 0) {

            std::cout << "Fizz" << std::endl;
            empty.signal();
            invalid.signal();            
        }
        m.unlock();              
    }
    invalid.broadcast();
    fill.broadcast();
    return NULL;
}

void *consumer_fizzBuzz(void *) {
      int tmp = 0;  
  
     while (tmp != -2) { 
        m.lock();
        while (num_full == 0) {     
            fill.wait(m);    
        }     
        tmp = do_get_fizzBuzz(); 

        if (tmp == -1) {
            invalid.signal();
            invalid.wait(m);
            invalid.signal();
        } else if (tmp > 0) {    
            std::cout << "FizzBuzz" << std::endl;
            empty.signal();  
            invalid.signal();
        }
        m.unlock();    
    } 
    invalid.broadcast();
    fill.broadcast();
    return NULL;
}

void *consumer_num(void *) {
    int tmp = 0;
  
     while (tmp != -2) { 
        m.lock();
        while (num_full == 0) {   
            fill.wait(m);      
        }  
        tmp = do_get();
        if(tmp == -1) {
            invalid.signal();
            invalid.wait(m);
            invalid.signal();
        } else if (tmp > 0) {   
        std::cout << tmp << std::endl;
        invalid.signal();
        empty.signal();

        }
        m.unlock();         
    } 
    invalid.broadcast();
    fill.broadcast();

    return NULL;
}


int main(int argc, char *argv[])
{
    if (argc != 3) {                                                            //if command line input invalid
        std::cout << "Usage ./fizzbuzz [number] [buffersize] " << std::endl;
        return 1;
    }
    loops = atoi(argv[1]);              //assign arguments
    max = atoi(argv[2]);

    buffer = (int *) malloc(max * sizeof(int));     //make buffer
    assert(buffer != NULL);

    int i;
    for (i = 0; i < max; i++) {                     //fill buffer
    buffer[i] = 0;
    }

    Thread pid;                                     //declare producers
    Thread pid2;

    pid.create(producer_odd);
    pid2.create(producer_even); 

    vector<Thread> cid(consumers);                              //create consumers
    cid[0].create(consumer_fizz, (void *) (long long int) 0);
    cid[1].create(consumer_buzz, (void *) (long long int) 1);
    cid[2].create(consumer_fizzBuzz, (void *) (long long int) 2);
    cid[3].create(consumer_num, (void *) (long long int) 3); 
                                                                //join all threads
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
    

    