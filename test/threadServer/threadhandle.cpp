﻿#include "threadhandle.h"

ThreadHandle::ThreadHandle()
{
    initfist = false;
}

ThreadHandle::~ThreadHandle() //停止所有线程，并释放资源
{
    QThread * tmp;
    for (auto it = threadSize.begin(); it != threadSize.end(); ++it)
    {
        tmp = it.key();
        if (tmp != QThread::currentThread()) {
            tmp->exit();
            tmp->wait(3000);
            delete tmp;
        }
    }
}

QThread *ThreadHandle::getThread()
{
    if (!initfist)
    {
        initThreadType(THREADSIZE,10);
    }
    if (type == THREADSIZE)
        return findThreadSize();
    else
        return findHandleSize();
}

void ThreadHandle::removeThread(QThread * thread)
{
    auto t = threadSize.find(thread);
    if (t != threadSize.end())
    {
        t.value() --;
        if (type == HANDLESIZE && t.value() == 0 && threadSize.size() > 1 && t.key() != QThread::currentThread())
        {
            mutex.lock();
            threadSize.remove(thread);
            thread->exit();
            thread->wait(3000);
            delete thread;
            mutex.unlock();
        }
    }
}

void ThreadHandle::initThreadType(ThreadType type, unsigned int max)
{
    if (!initfist)
    {
        this->type = type;
        this->size = max;
        if (this->size == 0) {
            if(type == THREADSIZE)
                this->size = 10;
            else
                this->size = 1000;
        }

        if (type == THREADSIZE) {
            initThreadSize();
        } else {
            mutex.lock();
            threadSize.insert(QThread::currentThread(),0);
            mutex.unlock();
        }
    }
    initfist = true;
}

void ThreadHandle::initThreadSize() //建立好线程并启动，
{
    QThread * tmp;
    mutex.lock();
    for (unsigned int i = 0; i < size -1 ;++i)
    {
        tmp = new QThread;
        threadSize.insert(tmp,0);
        tmp->start();
    }
    threadSize.insert(QThread::currentThread(),0);
    mutex.unlock();
}

QThread * ThreadHandle::findHandleSize() //查找到线程里的连接数小于最大值就返回查找到的，找不到就新建一个线程
{
    for (auto it  = threadSize.begin();it != threadSize.end() ;++it)
    {
        if (it.value() < size)
        {
            it.value() ++;
            return it.key();
        }
    }
    QThread * tmp = new QThread;
    mutex.lock();
    threadSize.insert(tmp,1);
    mutex.unlock();
    tmp->start();

    return tmp;
}

QThread *ThreadHandle::findThreadSize() //遍历查找所有线程中连接数最小的那个，返回
{
    auto it = threadSize.begin();
    auto ite = threadSize.begin();
    for (++it ; it != threadSize.end(); ++it)
    {
        if (it.value() < ite.value())
        {
            ite = it;
        }
    }
    ite.value() ++;
    return ite.key();
}

void ThreadHandle::clear()//仅仅清空计数，线程不释放
{
    for (auto it  = threadSize.begin();it != threadSize.end() ;++it)
    {
        it.value()  = 0;
    }
}
