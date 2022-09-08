#include "httpConn.h"
using namespace std;
int httpConn::m_epollFd = 0;
int httpConn::m_userCount = 0;
void httpConn::initConn(int sockfd,const sockaddr_in & address){
    curSocketFd = sockfd;
    curAddress = address;

    int reuse = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&reuse,sizeof(reuse));

    addEpollFd(m_epollFd,sockfd,true);    

    m_userCount++;
    readIndex = 0;
}
void httpConn::closeConn(){
    if(curSocketFd != -1){
        delEpollFd(curSocketFd);
        curSocketFd = -1;
        m_userCount--;
    }
}

bool httpConn::read(){
    cout<<"read success!!!"<<endl;
    if(readIndex >= readCount){
        return false;
    }
    int bytes_read = 0;
    while(true){
        bytes_read = recv(curSocketFd,readBuffer + readIndex,readCount - readIndex,0);
        if(bytes_read == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                //非阻塞读，当没有数据时可能会反悔上述两个其中一个
                break;
            }
            return false;
        }else if(bytes_read == 0){
            //对方关闭连接，
            return false;
        }
        readIndex += bytes_read;
    }
    printf("读取到数据为：\n %s\n",readBuffer);
    return true;
}
bool httpConn::write(){
    cout<<"write success!!!"<<endl;
    return true;
}
void httpConn::process(){
    //解析
    cout<<"解析数据"<<endl;
    HTTPCODE ret = process_read();
    if(ret == NO_REQUEST){
        modEpollFd(m_epollFd,curSocketFd,EPOLLIN);
        return;
    }

    //生成
    cout<<"生成数据"<<endl;

}