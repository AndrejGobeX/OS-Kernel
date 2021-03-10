#ifndef LIST_H
#define LIST_H

#include"quick.h"

template<class T>
class list
{
    public:
        list();
        virtual ~list();
        unsigned n;
        struct elem
        {
            elem *prev, *next;
            T* info;
            elem(T* _info):prev(0), next(0), info(_info){};
            ~elem()
            {
                info=0;
            }
        };
        elem *first, *last, *iter;
        void push_back(T* _info);
        void push_front(T* _info);
        void pop_back();
        void pop_front();
        T* operator[](int index);
        T* back();
        T* getById(unsigned Id);
        void erase(unsigned Id);
        void sort_insert(T* _info);
        unsigned size();
        void begin();
        T* current();
        void operator++();
};

template<class T>
list<T>::list()
{
    first=last=0;
    n=0;
}
template<class T>
T* list<T>::back()
{
    if(last==0)return 0;
    return last->info;
}
template<class T>
list<T>::~list()
{
    while(n>0)
        pop_back();
}
template<class T>
void list<T>::push_back(T* _info)
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
void list<T>::push_front(T* _info)
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
void list<T>::pop_back()
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
void list<T>::pop_front()
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
T* list<T>::operator[](int index)
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
T* list<T>::getById(unsigned Id)
{
    lock;
    elem *temp=first;
    while(temp!=0 && temp->info->id!=Id)
    {
        temp=temp->next;
    }
    if(temp==0){unlock; return 0;}
    unlock;
    return temp->info;
}
template<class T>
unsigned list<T>::size()
{
    return n;
}

template<class T>
void list<T>::erase(unsigned Id)
{
    elem *temp=first;
    while(temp!=0 && temp->info->id!=Id)
    {
        temp=temp->next;
    }
    if(temp==0)return;
    if(temp->prev!=0)temp->prev->next=temp->next;
    if(temp->next!=0)temp->next->prev=temp->prev;
    temp->next=0;
    temp->prev=0;
    temp->info=0;
    --n;
    delete temp;
}
template<class T>
void list<T>::sort_insert(T* _info)
{
    if(first==0)
    {
        first=last=new elem(_info);
    }
    else
    {
        elem *temp=first, *pre=0;
        while(temp!=0 && _info->sleepTime >= temp->info->sleepTime)
        {
            _info->sleepTime-=temp->info->sleepTime;
            pre=temp;
            temp=temp->next;
        }
        if(pre==0)
        {
            first->info->sleepTime-=_info->sleepTime;
            first->prev=new elem(_info);
            first->prev->next=first;
            first=first->prev;
        }
        else if(temp==0)
        {
            last->next=new elem(_info);
            last->next->prev=last;
            last=last->next;
        }
        else
        {
            temp->info->sleepTime-=_info->sleepTime;
            pre->next=new elem(_info);
            pre->next->prev=pre;
            pre->next->next=temp;
            temp->prev=pre->next;
        }
    }
    ++n;
}
template<class T>
void list<T>::begin()
{
    iter=first;
}
template<class T>
T* list<T>::current()
{
    if(iter==0)return 0;
    return iter->info;
}
template<class T>
void list<T>::operator++()
{
    if(iter!=0)
        iter=iter->next;
}

#endif // LIST_H
