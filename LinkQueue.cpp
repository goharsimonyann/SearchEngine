#include "linkQueue.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>

#undef _TRACE
#ifdef _TRACE
#define Trace(msg) std::cout << msg << std::endl;
#else
#define Trace(msg)
#endif

void* handlerInvoker(void* arg)
{
    linkQueue* LinkQueue = (linkQueue*)arg;
    while(linkQueue->_linkQueue.size())
    {
        pthread_mutex_lock(&linkQueue->_mutex);        
        std::string link = LinkQueue->_linkQueue.front();
        Trace(link);
        LinkQueue->_linkQueue.pop();
        Trace("Link removed from queue");         
        pthread_mutex_unlock(&LinkQueue->mutex);
       
         for(auto i = LinkQueue->_handlers.begin(); i != LinkQueue->_handlers.end(); ++i)
        {
            (*i)(link);
        }         
    }
}

void* msgReceiver(void* arg)
{
    Trace("Broadcaster thread started");
    linkQueue* LinkQueue = (linkQueue*)arg;
    while(true)
    {
        Trace("Waiting for link");
        pthread_cond_wait(&LinkQueue->condvar, &LinkQueue->mutex);
        Trace("Link received");
        pthread_t th;
        pthread_create(&th, NULL, handlerInvoker, arg);
        std::cout << "Queue size : " << LinkQueue->_linkQueue.size() << std::endl;
        std::cout << "Handler size : " << LinkQueue->_handlers.size() << std::endl;
       
    }
}

linkQueue::linkQueue()
{    
    pthread_create(&this->thread, nullptr, msgReceiver, (void*)this);
    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->condvar, NULL);
    sleep(1);
}

void linkQueue::addLink(std::string link)
{   
    std::cout << "This address : " << this << std::endl; 
    Trace("Locking mutex");
    pthread_mutex_lock(&mutex);
    Trace("Adding link");    
    this->_linkQueue.push(link);
    Trace("Link added");
    std::cout << "Queue size : " << LinkQueue->_linkQueue.size() << std::endl;
    pthread_cond_signal(&condvar);
    Trace("Cond var is signaled");
    pthread_mutex_unlock(&mutex);
}

void linkQueue::registerHandler(linkHandler handler)
{
    this->_handlers.insert(handler);
}

