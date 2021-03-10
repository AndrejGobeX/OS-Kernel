#include"event.h"
#include"quick.h"
#include"kerevent.h"

Event::Event(IVTNo ivtNo)
{
    disableInterrupts;
    myImpl=new KernelEv(ivtNo);
    enableInterrupts;
}
Event::~Event ()
{
    disableInterrupts;
    if(myImpl)delete myImpl;
    myImpl=0;
    enableInterrupts;
}
void Event::wait ()
{
    myImpl->wait();
}
void Event::signal()
{
    myImpl->signal();
}