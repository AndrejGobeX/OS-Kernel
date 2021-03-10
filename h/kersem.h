#ifndef _KERSEM_H_
#define _KERSEM_H_
#include "kernel.h"

class Semaphore;

class KernelSem
{
    public:
        static list<KernelSem> KernelSemList;
        KernelSem(int init, Semaphore* mySem);
        virtual ~KernelSem();
        int wait(unsigned int maxTimeToWait);
        int signal(int n=0);
        int val () const; // Returns the current value of the semaphore
        int id;
        static int GID;
        int sleepTime;
        Semaphore* mySemaphore;
        int v;
        list<PCB> blockedList;
        list<PCB> blockedWaitingList;
};

#endif