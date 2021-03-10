#ifndef slist_H
#define slist_H

#include"quick.h"

template<class T>
class slist
{
    public:
        slist();
        virtual ~slist();
        unsigned n;
        struct elem
        {
            elem *prev, *next;
            T info;
            elem(T _info):prev(0), next(0), info(_info){};
            ~elem()
            {
                info=0;
            }
        };
        elem *first, *last, *iter;
        void push_back(T _info);
        void push_front(T _info);
        void pop_back();
        void pop_front();
        T operator[](int index);
        T back();
        
        void erase();
        
        unsigned size();
        void begin();
        T current();
        void operator++();
        void swap(int c1, int c2);
};

template<class T>
slist<T>::slist()
{
    first=last=0;
    n=0;
}
template<class T>
T slist<T>::back()
{
    if(last==0)return 0;
    return last->info;
}
template<class T>
slist<T>::~slist()
{
    while(n>0)
        pop_back();
}
template<class T>
void slist<T>::push_back(T _info)
{
    if(n==0)
    {
        first=last=new elem(_info);
    }
    else
    {
        last->next=new elem(_info);
        last->next->prev=last;
        last=last->next;
    }
    ++n;
}
template<class T>
void slist<T>::push_front(T _info)
{
    if(n==0)
    {
        first=last=new elem(_info);
    }
    else
    {
        first->prev=new elem(_info);
        first->prev->next=first;
        first=first->prev;
    }
    ++n;
}
template<class T>
void slist<T>::pop_back()
{
    if(n==0)return;
    if(n==1)
    {
        delete last;
        first=last=0;
    }
    else
    {
        last=last->prev;
        delete last->next;
        last->next=0;
    }
    --n;
}
template<class T>
void slist<T>::pop_front()
{
    if(n==0)return;
    if(n==1)
    {
        delete first;
        first=last=0;
    }
    else
    {
        first=first->next;
        delete first->prev;
        first->prev=0;
    }
    --n;
}
template<class T>
T slist<T>::operator[](int index)
{
    if(index>=n)return 0;
    elem *temp=first;
    for(int i=0; i<index; ++i)
    {
        temp=temp->next;
    }
    return temp->info;
}
template<class T>
unsigned slist<T>::size()
{
    return n;
}

template<class T>
void slist<T>::erase()
{
    elem *temp=iter;
    if(temp==0)return;
    if(temp->prev!=0)temp->prev->next=temp->next;
    if(temp->next!=0)temp->next->prev=temp->prev;
    iter=temp->prev;
    temp->next=0;
    temp->prev=0;
    temp->info=0;
    --n;
    delete temp;
    
}
template<class T>
void slist<T>::begin()
{
    iter=first;
}
template<class T>
T slist<T>::current()
{
    if(iter==0)return 0;
    return iter->info;
}
template<class T>
void slist<T>::operator++()
{
    if(iter!=0)
        iter=iter->next;
}
template<class T>
void slist<T>::swap(int c1, int c2)
{
    if(c2>=n || c1>=n)return;
    elem* t1=first, *t2=first;
    while(c1>0)t1=t1->next, --c1;
    while(c2>0)t2=t2->next, --c2;
    T v;
    v=t1->info;
    t1->info=t2->info;
    t2->info=v;
}

#endif // slist_H
