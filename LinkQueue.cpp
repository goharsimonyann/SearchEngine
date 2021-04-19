#include "LinkQueue.h"
#include <pthread.h>

void* msgReceiver(void* arg)
{
    linkQueue* urlQueue = (linkQueue*)arg;
    while(true)
    {
        pthread_cond_wait(&urlQueue->condvar, &urlQueue->mutex);
        std::string link = urlQueue->_linkQueue.front();
        urlQueue->_linkQueue.pop();
        pthread_mutex_unlock(&urlQueue->mutex);

        for(auto i = urlQueue->_handlers.begin(); i != urlQueue->_handlers.end(); ++i)
        {
            (*i)(link);
        }
    }
}

linkQueue::linkQueue()
{
    pthread_create(&this->thread, nullptr, msgReceiver, this);
}

void linkQueue::addLink(std::string link)
{
    pthread_mutex_lock(&this->mutex);
    this->_linkQueue.push(link);
    pthread_cond_signal(&this->condvar);
    pthread_mutex_unlock(&this->mutex);

}

void linkQueue::registerHandler(linkHandler handler)
{
    this->_handlers.insert(handler);
}

