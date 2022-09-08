#ifdef HTTPCONNECTION_H
#define HTTPCONNECTION_H
#include "networkHeader.h"
#include "Utils.h"



class httpConn{
public:
    static int m_epollFd ;
    static int m_userCount ;
    static int readCount = 2048;
    static int writeCount = 2048;

    httpConn();
    ~httpConn();
    
    enum METHOD = {GET = 0,POST,PUT,HEADER,DELETE};
    enum CHECKSTATE = {CHECK_STATE_REQUESTLINE = 0,CHECK_STATE_HEADER,CHECK_STATE_CONTENT};
    enum LINESTATUS = {LINE_OK=0,LINE_OPEN,LINE_BAD};
    enum HTTPCODE = {NO_REQUEST,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FORBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECTION};

     

    void initConn(int sockfd,const sockaddr_in & address);
    void process(); //处理请求
    void closeConn();

    bool read();
    bool write();

    //解析http请求
    HTTPCODE process_read();
    HTTPCODE process_read_Line();
    HTTPCODE process_read_Header();
    HTTPCODE process_read_Content();

    LINESTATUS  process_read_line();
    //生成http响应

private:
    int curSocketFd;
    sockaddr_in curAddress;

    char readBuffer[readCount];
    char writeBuffer[writeCount];
    int readIndex;

};
#endif