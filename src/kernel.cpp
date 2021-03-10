#include "kernel.h"
#include "kersem.h"
#include "ivtentry.h"
volatile PCB* running;
int PCB::Gsigflags[16]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
PCB::~PCB()
{
    lock;
    if(stack!=0)
    {
        delete [] stack;
        stack=0;
    }
    id=-1;
    state=UNDEF;
    if(myThread)myThread->myPCB=0;
    myThread=0;
    unlock;
}

PCB::PCB()
{
    lock;
    father=0;
    stack=0;
    sp=ss=bp=0;
    state=READY;
    id=GID++;
    timeSlice=defaultTimeSlice;
    myThread=0;
    sleepTime=-1;
    for(int i=0; i<16; ++i)sigflags[i]=1;
    unlock;
}

PCB::PCB(StackSize stackSize, Time _timeSlice, Thread *thread, void (*body)(), State _state)
{
    lock;
    father=(PCB*)running;

    if(stackSize<256)stackSize=256;
    if(stackSize>65536)stackSize=65536;

    stackSize/=sizeof(unsigned);
	stack = new unsigned[stackSize];

	stack[stackSize-1] = 0x200;
	stack[stackSize-2] = FP_SEG(body);
	stack[stackSize-3] = FP_OFF(body);

    sp = FP_OFF(stack+stackSize-12);
	ss = FP_SEG(stack+stackSize-12);
    bp = sp;
	state = _state;
    id=GID++;
    timeSlice=_timeSlice;
    myThread=thread;
    sleepTime=-1;
    for(int i=0; i<16; ++i)sigflags[i]=1;
    unlock;
}

void PCB::wrapper()
{
    running->myThread->run();
    lock;
    while(running->waitingList.size()>0)
    {
        running->waitingList[0]->state=READY;
        Scheduler::put(running->waitingList[0]);
        running->waitingList.pop_front();
    }

    if(running->father!=0)
    {
        running->father->signal(1);
    }
    running->signal(2);
    running->handleSignals();

    running->state=FINISHED;
    unlock;
    dispatch();
}
list<PCB> PCBList;

void PCB::signal(SignalId signal)
{
    lock;
    if(signal<16)
    {
        signalQueue.push_back(signal);
    }
    unlock;
}
void PCB::registerHandler(SignalId signal, SignalHandler handler)
{
    lock;
    if(signal<16)
    {
        handlers[signal].push_back(handler);
    }
    unlock;
}
void PCB::unregisterAllHandlers(SignalId id)
{
    lock;
    if(id<16)
    {
        while(handlers[id].size()>0)
        {
            handlers[id].pop_front();
        }
    }
    unlock;
}
void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
    if(id>15)return;
    lock;
    int i;
    int c1=-1;
    handlers[id].begin();
    for(i=0; i<handlers[id].size(); ++i)
    {
        if(handlers[id].current()==hand1)
        {
            c1=i;
            break;
        }
        ++handlers[id];
    }
    int c2=-1;
    handlers[id].begin();
    for(i=0; i<handlers[id].size(); ++i)
    {
        if(handlers[id].current()==hand2)
        {
            c2=i;
            break;
        }
        ++handlers[id];
    }
    if(c1>=0 && c2>=0)
        handlers[id].swap(c1, c2);
    unlock;
}
void PCB::blockSignal(SignalId signal)
{
    lock;
    sigflags[signal]=0;
    unlock;
}
void PCB::blockSignalGlobally(SignalId signal)
{
    lock;
    Gsigflags[signal]=0;
    unlock;
}
void PCB::unblockSignal(SignalId signal)
{
    lock;
    sigflags[signal]=1;
    unlock;
}
void PCB::unblockSignalGlobally(SignalId signal)
{
    lock;
    Gsigflags[signal]=1;
    unlock;
}

int PCB::handleSignals()
{
    int sig;
    int sz=signalQueue.size();
    for(int sq=0; sq<sz; ++sq)
    {
        sig=signalQueue[0];
        signalQueue.pop_front();
        if(sigflags[sig]==0 || Gsigflags[sig]==0)
        {
            signalQueue.push_back(sig);
            continue;
        }
        if(sig==0)return 1;
        handlers[sig].begin();
        for(int i=0; i<handlers[sig].size(); ++i, ++handlers[sig])
        {
            ( *(handlers[sig].current()) )();
        }
    }
    return 0;
}

void PCB::kill(PCB* pcb)
{
    pcb->state=FINISHED;
    pcb->myThread->myPCB=0;
    lock;
    while(pcb->waitingList.size()>0)
    {
        pcb->waitingList[0]->state=READY;
        Scheduler::put(pcb->waitingList[0]);
        pcb->waitingList.pop_front();
    }
    unlock;
    PCBList.erase(pcb->id);
    delete pcb;
    pcb=0;
}

void idlefun()
{
    while(running!=0);
}

PCB driverPCB, idlePCB(0, 1, 0, idlefun, UNDEF);//id:0 main, id:1 idle
ID PCB::GID=0;

void deletePCBs()
{
    PCB* temp;
    while(PCBList.size()>0)
    {
        temp=(PCBList[0]);
        if(temp!=0)delete temp;
        PCBList.pop_front();
    }
}
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int timeCounter=20;
volatile int intent=0;
volatile int lockCnt=0;

void tick();
KernelSem *tempSem;
volatile PCB* killed=0;
void interrupt timer(...){

    if(!intent) asm int 60h;
	if (!intent)
    {
        timeCounter--;
        if(timeCounter==-2)timeCounter=-1;
        tick();
        KernelSem::KernelSemList.begin();
        while(KernelSem::KernelSemList.current()!=0)
        {
            tempSem=KernelSem::KernelSemList.current();
            tempSem->blockedWaitingList[0]->sleepTime--;
            while(tempSem->blockedWaitingList.size()>0 && tempSem->blockedWaitingList[0]->sleepTime<=0)
            {
                tempSem->blockedWaitingList[0]->state=READY;
                tempSem->blockedWaitingList[0]->timeWake=0;
                Scheduler::put(tempSem->blockedWaitingList[0]);
                tempSem->blockedWaitingList.pop_front();
                tempSem->v++;
            }
            ++KernelSem::KernelSemList;
        }
    }
	if (timeCounter == 0 || intent) {
        if(!locked){
            intent=0;
		asm {
			mov tsp, sp
			mov tss, ss
            mov tbp, bp
		}

		running->sp = tsp;
		running->ss = tss;
        running->bp = tbp;

        if(running->state==READY)Scheduler::put((PCB*)running);

        killed=0;
        do
        {
            running=Scheduler::get();
            if(running==0)running=&idlePCB;

            tsp = running->sp;
            tss = running->ss; 
            tbp = running->bp;

            timeCounter = running->timeSlice;

            asm {
                mov sp, tsp
                mov ss, tss
                mov bp, tbp
            }

            if(killed!=0)
            {
                PCB::kill((PCB*)killed);
                killed=0;
            }
            if(running->state==UNDEF)break;
            if(running->handleSignals()==1)
            {
                killed=(PCB*)running;
            }
        }while(killed!=0);

        }
        else{intent=1;}    
	} 
}

void dispatch()
{
	disableInterrupts;
	intent = 1;
	asm int 8;
	enableInterrupts;
}

unsigned oldTimerOFF, oldTimerSEG;
//pInterrupt oldTimer;
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax	
		mov ax, word ptr es:0020h	
		mov word ptr oldTimerOFF, ax	

		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
    /*disableInterrupts;
    oldTimer=getvect(8);
    setvect(0x60, oldTimer);
    setvect(8, timer);
    enableInterrupts;*/
}

void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
    /*disableInterrupts;
    setvect(8, oldTimer);
    enableInterrupts;*/
}

void exitThread(){
	running->state = FINISHED;
	dispatch();
}

void clearInterrupts(){
    disableInterrupts;
    for(int i=0; i<256; ++i)
    {
        if(IVTEntry::Array[i]!=0)
        {
            delete IVTEntry::Array[i];
            IVTEntry::Array[i]=0;
        }
    }
    enableInterrupts;
}