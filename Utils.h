//RAII 封装一层 
//
#ifndef UTILS_H
#define UTILS_H
#include<iostream>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include "networkHeader.h"
#include<exception>
class  MyMutex{
public:
    MyMutex(){
        if( pthread_mutex_init(&myMutex,nullptr)  != 0){
            throw std::exception();
        }
    }
    ~MyMutex(){
        pthread_mutex_destroy(&myMutex);
    }
    bool lock(){
        return pthread_mutex_lock(&myMutex) == 0;
    }
    bool unlock(){
        return pthread_mutex_unlock(&myMutex) == 0;
    }
private:
    pthread_mutex_t myMutex;
};
class MyCondSem{
public:
    MyCondSem(){
        if(pthread_cond_init(&myCondSem,nullptr) != 0){
            throw std::exception();
        }
    }
    ~MyCondSem(){
        pthread_cond_destroy(&myCondSem);
    }
    bool wait(pthread_mutex_t mutex){
        return pthread_cond_wait(&myCondSem,&mutex) == 0; 
    }
    bool timeWait(pthread_mutex_t *mutex, struct timespec * t){
        return pthread_cond_timedwait(&myCondSem,mutex,t);
    }
    bool signal(){
        return pthread_cond_signal(&myCondSem) == 0;
    }
    bool broadcast(){
        return pthread_cond_broadcast(&myCondSem) == 0;
    }
    
private:
    pthread_cond_t myCondSem;
};
class MySem{
public:
    MySem(){
        if(sem_init(&mySem,0,0) != 0){
            throw std::exception();
        }
    }
    MySem(int num){
        if(sem_init(&mySem,0,num) != 0){
            throw std::exception();
        }
    }
    ~MySem(){
        sem_destroy(&mySem);
    }
    bool wait(){
        return sem_wait(&mySem) == 0;
    }
    bool post(){
        return sem_post(&mySem) == 0;
    }

private:
    sem_t mySem;
};

void addSignal(int sigNum,void(handler)(int)){
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sigaction(sigNum,&sa,NULL);
}

void addEpollFd(int epollfd,int fd,bool oneShot){
    epoll_event event;
    event.data.fd =  fd;
    event.events = EPOLLIN | EPOLLRDHUP;
    if(oneShot){
        event.events | EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
}; 

void delEpollFd(int epollfd,int fd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}
#endif

