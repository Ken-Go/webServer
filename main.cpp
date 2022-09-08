#include "networkHeader.h"
#include "Utils.h"
#include "threadPool.h"
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
    int epollfd = 




    return 0;
}