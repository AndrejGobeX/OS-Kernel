#include"thread.h"
#include"kernel.h"

void Thread::start()
{
    if(myPCB==0)return;
    lock;
    myPCB->state=READY;
    Scheduler::put(myPCB);
    unlock;
}
void Thread::waitToComplete()
{
    lock;

    if(myPCB==(PCB*)running || myPCB==0 || myPCB->id==1 || myPCB->state==CREATED || myPCB->state==FINISHED || myPCB->state==UNDEF || myPCB->id==0)
    {
		unlock;
		return;
	}
    running->state=BLOCKED;
    myPCB->waitingList.push_back((PCB*)running);

    unlock;
    dispatch();
}
Thread::~Thread()
{
    waitToComplete();
    lock;
    if(myPCB!=0)
    {
        PCBList.erase(myPCB->id);
        delete myPCB;
    }
    myPCB=0;
    unlock;
}
ID Thread::getId()
{
    if(myPCB==0)return -1;
    return myPCB->id;
}
ID Thread::getRunningId()
{
    return running->id;
}
Thread * Thread::getThreadById(ID id)
{
    lock;
    PCB* p=PCBList.getById(id);
    if(p==0){unlock;return 0;}
    unlock;
    return p->myThread;
}
Thread::Thread (StackSize stackSize, Time timeSlice)
{
    lock;
    myPCB=new PCB(stackSize, timeSlice, this, PCB::wrapper);
    PCBList.push_back(myPCB);
    unlock;
}
void Thread::signal(SignalId signal)
{
    if(myPCB==0)return;
    myPCB->signal(signal);
}
void Thread::registerHandler(SignalId signal, SignalHandler handler)
{
    if(myPCB==0)return;
    myPCB->registerHandler(signal, handler);
}
void Thread::unregisterAllHandlers(SignalId id)
{
    if(myPCB==0)return;
    myPCB->unregisterAllHandlers(id);
}
void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
    if(myPCB==0)return;
    myPCB->swap(id, hand1, hand2);
}
void Thread::blockSignal(SignalId signal)
{
    if(myPCB==0)return;
    myPCB->blockSignal(signal);
}
void Thread::blockSignalGlobally(SignalId signal)
{
    PCB::blockSignalGlobally(signal);
}
void Thread::unblockSignal(SignalId signal)
{
    if(myPCB==0)return;
    myPCB->unblockSignal(signal);
}
void Thread::unblockSignalGlobally(SignalId signal)
{
    PCB::unblockSignalGlobally(signal);
}