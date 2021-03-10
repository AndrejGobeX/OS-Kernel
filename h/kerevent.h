#ifndef _KEREVENT_H_
#define _KEREVENT_H_

class PCB;
class Semaphore;
class KernelEv
{
    public:
    KernelEv(unsigned char _ivtNo);
    ~KernelEv();
    void wait();
    void signal();
    unsigned char ivtNo;
    PCB* owner;
    Semaphore* semaphore;
};

#endif