#include"kerevent.h"
#include"ivtentry.h"
#include"kernel.h"
#include"semaphor.h"

KernelEv::KernelEv(unsigned char _ivtNo)
{
    disableInterrupts;
    ivtNo=_ivtNo;
    IVTEntry::Array[ivtNo]->myKernelEv=this;
    owner=(PCB*)running;
    semaphore=new Semaphore(0);
    enableInterrupts;
}
KernelEv::~KernelEv()
{
    disableInterrupts;
    IVTEntry::Array[ivtNo]->myKernelEv=0;
    signal();
    if(semaphore)delete semaphore;
    semaphore=0;
    enableInterrupts;
}
void KernelEv::wait()
{
    disableInterrupts;
    if(owner==(PCB*)running && owner->state==READY)
    {
        semaphore->wait(0);
    }
    enableInterrupts;
}
void KernelEv::signal()
{
    disableInterrupts;
    if(semaphore->val()<0)
    {
        semaphore->signal(0);
    }
    enableInterrupts;
}