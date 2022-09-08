#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "Utils.h"
#include <vector>
#include <list>
static int maxThreadNum = 10;
static int maxRequestNum = 10000;
template<typename T>
class MyThreadPool{
public:
    MyThreadPool(int threadNum,int requestNum);
    MyThreadPool();
    ~MyThreadPool();
    bool append(T* request);
    void run();
private:
static void * work(void * arg);
private:
    MyMutex myMutex;
    std::vector<pthread_t*> myThreads;
    std::list<T*> myWorks; 
    MySem myRequestSem;
    bool stop;
};

template<typename T>
MyThreadPool<T>::MyThreadPool(int threadNum,int requestNum){
    if(threadNum <= 0 || threadNum > maxThreadNum ){
        cout<<"threadNum is not correct!!!"<<endl;
        throw std::exception();
    }
    if(requestNum <= 0 || requestNum > maxRequestNum ){
        cout<<"RequestNum is not correct!!!"<<endl;
        throw std::exception();
    }
    stop = false;
    myThreads.resize(threadNum);
    for(int i =0;i < threadNum;i++){
        if(pthread_create(&myThreads[i],nullptr,&work,this) != 0){
            cout<<"threadPool::create thead num :"<< i << "failure!!!"<<endl;
            throw std::exception();
        };
        if(pthread_detach(&myThreads[i]) != 0){
            cout<<"threadPool::detach thead num :"<< i << "failure!!!"<<endl;
            throw std::exception();
        };
    }
};

template<typename T>
MyThreadPool<T>::MyThreadPool(){
    stop = false;
    myThreads.resize(8);
    for(int i =0;i < 8;i++){
        if(pthread_create(&myThreads[i],nullptr,&work,nullptr) != 0){
            cout<<"threadPool::create thead num :"<< i << "failure!!!"<<endl;
            throw std::exception();
        };
    }
};

template<typename T>
MyThreadPool<T>::~MyThreadPool(){
    stop = true;
};

template<typename T>
bool MyThreadPool<T>::append(T* request){
      myMutex.lock();
    if(myWorks.size() >= maxRequestNum){
         myMutex.unlock();
        cout<<"threadPool::append queue has full!!"<<endl;
        return false;
    }
    myWorks.insert(request);
    myMutex.unlock();
    myRequestSem.post();
    return true;
};

template<typename T>
void * MyThreadPool<T>::work(void * arg){
    MyThreadPool * myThreadPool =  (MyThreadPool *) arg;
    myThreadPool->run();
    return myThreadPool;
};  

template<typename T>
void MyThreadPool<T>::run(){
    while (!stop)   
    {
        myRequestSem.wait();
        myMutex.lock();
        if(myWorks.size() == 0){
            myMutex.unlock();
            continue;
        }
        T* request = myWorks.front();
        myWorks.pop_front();
        myMutex.unlock();
        if(!request){
            continue;
        } 
        request->process();
    }
    
};  
#endif