# yocto-assignments-base
Base repository for AESD Yocto assignments


Note:

This is a fork implementation of the assignment. As there is no default implementation of pthreads and 
threads are simply a process that is busy waiting I have rationalized that it would be better to use
simple fork. From my understanding, a process that is in sleep is spent counting its program counter
until it is taken out of sleep. This is different than a kernel sleep that also needs a controlling 
process.




