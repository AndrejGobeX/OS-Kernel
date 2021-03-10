#include"kersem.h"
#include"semaphor.h"
list<KernelSem> KernelSem::KernelSemList;
int KernelSem::GID=0;

KernelSem::KernelSem(int init, Semaphore* mySem)
{
    id=KernelSem::GID++;
    mySemaphore=mySem;
    v=init;
}
KernelSem::~KernelSem()
{
    mySemaphore=0;
    signal(-val());
}
int KernelSem::wait(unsigned int maxTimeToWait)
{
    if(--v>=0)return 1;
    disableInterrupts;
    running->timeWake=1;
    if(maxTimeToWait==0)
    {
        running->state=BLOCKED;
        blockedList.push_back((PCB*)running);
        dispatch();
        enableInterrupts;
        return running->timeWake;
    }
    else
    {
        running->state=BLOCKED;
        running->sleepTime=maxTimeToWait;
        blockedWaitingList.sort_insert((PCB*)running);
        dispatch();
        enableInterrupts;
        return running->timeWake;
    }
}
int KernelSem::signal(int n)
{
    if(n<0)return n;
    disableInterrupts;
    int t=0;
    if(n==0)n=1;
    while(blockedList.size()>0 && t<n)
    {
        blockedList[0]->state=READY;
        Scheduler::put(blockedList[0]);
        blockedList.pop_front();
        ++t;
    }
    while(blockedWaitingList.size()>0 && t<n)
    {
        blockedWaitingList[0]->state=READY;
        Scheduler::put(blockedWaitingList[0]);
        blockedWaitingList.pop_front();
        ++t;
    }
    v+=n;
    enableInterrupts;
    return t;
}
int KernelSem::val () const
{
    return v;
}