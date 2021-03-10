#ifndef _KERNEL_H_7
#define _KERNEL_H_7

#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "quick.h"
#include "thread.h"
#include "list.h"
#include "slist.h"

class PCB{
public:
    static ID GID;
    unsigned bp;
	unsigned sp;
	unsigned ss;
	State state;
	Time timeSlice;
    Time sleepTime;
    int timeWake;
    ID id;
    Thread *myThread;
    list<PCB> waitingList;
    unsigned *stack;
    PCB();
    PCB(StackSize stackSize, Time _timeSlice, Thread *thread, void (*body)(), State _state=CREATED);
    ~PCB();
    static void wrapper();

    void signal(SignalId signal);
    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId id);
    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
    void blockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);
    void unblockSignal(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);

    int handleSignals();
    static void kill(PCB* pcb);

    slist<SignalHandler> handlers[16];
    int sigflags[16];
    static int Gsigflags[16];
    slist<unsigned> signalQueue;
    PCB* father;

};
void clearInterrupts();
void deletePCBs();

void interrupt timer(...);

void dispatch();

//pInterrupt oldTimer;
void inic();

void restore();

void exitThread();
#endif