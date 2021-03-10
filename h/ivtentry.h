#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_
#include"quick.h"

class KernelEv;

class IVTEntry
{
    public:
    IVTEntry(unsigned char _ivtNo, void interrupt (*f)(...));
    ~IVTEntry();
    void attach();
    void detach();
    void old();
    unsigned char ivtNo;
    pInterrupt oldFun, newFun;
    KernelEv* myKernelEv;
    void signal();
    static IVTEntry* Array[256];
};

#define PREPAREENTRY(No, keep)\
void interrupt interruptFun##No(...);\
IVTEntry ivte##No(No, interruptFun##No);\
void interrupt interruptFun##No(...){\
    ivte##No.signal();\
    if(keep)ivte##No.old();\
    dispatch();\
}

#endif