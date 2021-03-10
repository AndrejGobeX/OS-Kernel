#include"ivtentry.h"
#include<dos.h>
#include"kerevent.h"

IVTEntry* IVTEntry::Array[256]={0};

IVTEntry::IVTEntry(unsigned char _ivtNo, void interrupt (*f)(...))
{
    disableInterrupts;
    ivtNo=_ivtNo;
    newFun=f;
    myKernelEv=0;
    attach();
    IVTEntry::Array[ivtNo]=this;
    enableInterrupts;
}
IVTEntry::~IVTEntry()
{
    disableInterrupts;
    detach();
    IVTEntry::Array[ivtNo]=0;
    myKernelEv=0;
    enableInterrupts;
}
void IVTEntry::attach()
{
    disableInterrupts;
    oldFun=getvect(ivtNo);
    setvect(ivtNo, newFun);
    enableInterrupts;
}
void IVTEntry::detach()
{
    disableInterrupts;
    setvect(ivtNo, oldFun);
    if(oldFun!=0)oldFun();
    enableInterrupts;
}
void IVTEntry::signal()
{
    if(myKernelEv!=0)
    {
        myKernelEv->signal();
    }
}
void IVTEntry::old()
{
    disableInterrupts;
    (*oldFun)();
    enableInterrupts;
}