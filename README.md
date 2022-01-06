
**CPSC 351 - Fall 2021 - Project 4 - Working with the Thread API**

**Group Members**
Clay Golan: clayg@csu.fullerton.edu
Sean McCarthy: spmccarthy4@csu.fullerton.edu
Malka Lazerson: mlazerson@csu.fullerton.edu

**Project Description**
This project will solve a common programming puzzle, Fizz Buzz, using threads and condition variables.  Fizz buzz is a game to teach 
division to children, and is often used in the industry to test programmers (the reasoning may not be the most sound). Players take turns 
to count incrementally, replacing any number divisible by three with the word "fizz", and any number divisible by five with the word 
"buzz". 

The programs should be written in C++ and run on Tuffix using either the GCC or Clang compiler. The program will accept numbers and other 
arguments via the command line. These programs will solve the puzzle using an increasing number of threads, producers, and consumers. 
Producer threads will place numbers to check into a bounded buffer, and consumer threads will check the numbers and produce output.

**Documentation**

*Fizzbuzz*

If the user does not input a number, the program will output a usage message...

"Usage: ./fizzbuzz {number} "

Otherwise, the program will take the 3rd element of the argument vector and convert it from a char to a number. The program will start 
from 1 and increment "i" until it reaches {number}. Output will depend on the following conditions....

If i % 3 == 0, fizz is the output.

If i % 5 == 0, buzz is the output.

If i % 15 == 0, fizzbuzz is the output.

Else, i is the output 

*Producer-Consumer*

Now our solution will be extended to use two threads, a producer and a consumer.

*do_fill* is used to put a number in the buffer, and increments the pointer. *do_get* retrieves the number from the buffer and decrements 
the pointer. Both fucntions also track how full the buffer is at any given time. 

The producer thread increments up to {number}, placing each integer in the bounded buffer. The consumer thread takes the integer from the 
buffer, and prints an output corresponding to the Fizzbuzz logic....  

If i % 3 == 0, fizz is the output.

If i % 5 == 0, buzz is the output.

If i % 15 == 0, fizzbuzz is the output.

Else, i is the output 

The Consumer waits on an empty buffer and the producer waits on a full buffer. The program is terminated when the correct number of 
values have been produced. (for loop)

*Multiple Producers*

Our solution will now extend to two threads, one producing odd numbers, and the other producing even numbers. There is one consumer to 
handle the actual Fizzbuzz logic and output.

Now, the *producer_odd* produces odd increments (1,3,5, etc.) to place in the bounded buffer, and the *producer_even* will produce the 
even increments (2,4,6, etc.) to place in the bounded buffer. 

*do_fill* is used to put the odd OR even (depending on the producer that calls it) increment in the buffer, and *do_get* retrieves the 
number from the buffer for the one consumer. 

The lone consumer will take the increment and let the internal logic decide the output. The output logic follows as before....

If i % 3 == 0, fizz is the output.

If i % 5 == 0, buzz is the output.

If i % 15 == 0, fizzbuzz is the output.

Else, i is the output 

This program follows the same end case logic of the previous, using a for loop to track how many numbers have been produced.

*Multiple Consumers*

Our solution will again be extended to four threads: one to print Fizz, one to print Buzz, one to print FizzBuzz, and one to print the 
other integers.

There is still one *producer_odd* producing odd increments and one *producer_ even* producing even increments. There are now four 
consumers, *consumer_fizz*, *consumer_buzz*, *consumer_fizzbuzz*, and *consumer_num*.

Each consumer thread will use a seperate get function, that corrosponds to its case. The get function will test if the value in the 
buffer is valid to be consumed by the thread. If it is, it will consume it. If not it will put the thread to sleep and another will try.

In this program once both producers are done producing, they place a single -2 value in the bounded buffer. When a consumer thread sees  
a -2 it keeps the use_ptr pointed at the -2 so all other consumer threads will also see it. When a consumer thread consumes a -2 it 
finishes. 

*Ordering Output*

Since thread scheduling is nondeterministic, the previous two solutions do not always produce output in the same order. This program 
attempts, meaning it is not fully working, to fix that problem using an ordering variable and ordering condition variable. 


**Files**

Makefile

Fizzbuzz: Solves the Fizzbuzz problem

Producer-Consumer: Solves the Fizzbuzz problem using a producer to produce increments and a consumer to act on them

Multiple-Producers: Solves the Fizzbuzz problem using an odd number producer and even number producer, with one consumer to act on the 
increments.

Multiple-Consumers: Solves the Fizzbuzz problem using an odd number producer and even number producer, with four consumers to act on the 
increments.

Ordering-Output: Solves the Fizzbuzz problem by producing output in the same order, where the odd and even producers switch off. 

**Errors, Setbacks, etc.**
We have been unsuccessful in implementing Ordering-Output. We tried to force the two producer threads to alternate their production thus 
placing the numbers into the buffer in order. We tried using a lock and 2 condition variables however have been unable to not do so 
without encoundtering deadlock of some kind. We also have been unable to successfully order the outputs while trying this method. 


**Tarball Contents**
1.  A README file 
2.  The source code for each utility in C++17
3.  A Makefile 

> Written with [StackEdit](https://stackedit.io/).

