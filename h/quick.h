#ifndef _QUICK_H_
#define _QUICK_H_

#define disableInterrupts asm{ pushf; cli;}
#define enableInterrupts asm{ popf;}

typedef int bool;
#define true 1
#define false 0

void dispatch();

class PCB;
template<class T>
class list;

extern int syncPrintf(const char* format, ...);

extern volatile int lockCnt;
extern volatile bool changeReady, intent;
extern volatile PCB* running;
extern list<PCB> PCBList;
extern PCB driverPCB, idlePCB;

#define lock ++lockCnt;
#define unlock if(--lockCnt && intent){dispatch();}
#define locked (lockCnt>0)

typedef void interrupt (*pInterrupt)(...);
typedef void (*function)();

enum State{RUNNING, SLEEP, BLOCKED, FINISHED, READY, UNDEF, CREATED};

#endif