#include "networkHeader.h"
#include "Utils.h"
#include "threadPool.h"
#include "sys/epoll.h"
#include "httpConn.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
static const int maxFd = 65535;
static const int maxEventNum = 10000;


int main(int argc, char* argv[]){
    if(argc <= 1){
        cout<<"请按照如下指令运行程序："<< basename(argv[0])<<" $port_number"<<endl;
        exit(-1);
    }

    int port =atoi(argv[1]);
    addSignal(SIGPIPE,SIG_IGN);
    MyThreadPool<httpConn> * pool = nullptr; 
    try{
        pool = new MyThreadPool<httpConn>();
    }catch(...){
        cout<<"创建线程池失败。。。。。！！"<<endl;
        exit(-1);
    }

    // 
    vector<httpConn> users(maxFd);
    //创建socket fd
    int listenFd = socket(PF_INET,SOCK_STREAM,0);

    //设置端口复用
    int reuse = 1;
    setsockopt(listenFd,SOL_SOCKET,SO_REUSEPORT,&reuse,sizeof(reuse));

    //bind 
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr  = INADDR_ANY;
    address.sin_port = htons(port);
    bind(listenFd,(struct sockaddr*)&address,sizeof(address));

    accept(listenFd,5);

    epoll_event events[maxEventNum];
    int epollfd = epoll_create(5);

    addEpollFd(epollfd,listenFd,false);

    httpConn::m_epollfd = epollfd;

    while(true){
        int num = epoll_wait(epollfd,events,maxEventNum,-1);
        if(num < 0 && errno != EINTR){
            cout<<"epoll failure"<<endl;
            break;
        }
        for(int i =0; i < num;i++){
            int sockfd = events[i].data.fd;
            if(sockfd == listenFd){
                struct sockaddr_in inAddress;
                socklen_t inAddressLen = sizeof(inAddress);
                int connfd = accept(listenFd,(struct sockaddr *)inAddress,&inAddressLen);
                if(httpConn::m_userCount >= maxFd){
                    //目前连接数满了，回复500，服务器正忙
                    close(connfd);
                }
                users[connfd].init(connfd,inAddress);

            }else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                users[sockfd].closeConn();
            }else if(events[i].events & EPOLLIN){
                if(users[sockfd].read()){
                    pool->append(users[sockfd]);
                }else{
                    users[sockfd].closeConn();
                }
            }else if(events[i].events & EPOLLOUT){
                if(!user[sockfd].write()){
                    users[sockfd].closeConn();
                }
            }
        }
    }

    close(epollfd);
    close(listenFd);
    return 0;
}