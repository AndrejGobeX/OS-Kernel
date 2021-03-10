#include"semaphor.h"
#include"kersem.h"

Semaphore::Semaphore (int init)
{
    disableInterrupts;
    if(init<0)init=0;
    myImpl=new KernelSem(init, this);
    KernelSem::KernelSemList.push_back(myImpl);
    enableInterrupts;
}
Semaphore::~Semaphore ()
{
    disableInterrupts;
    KernelSem::KernelSemList.erase(myImpl->id);
    delete myImpl;
    myImpl=0;
    enableInterrupts;
}
int Semaphore::wait (Time maxTimeToWait)
{
    return myImpl->wait(maxTimeToWait);
}
int Semaphore::signal(int n)
{
    return myImpl->signal(n);
}
int Semaphore::val () const
{
    return myImpl->val();
}