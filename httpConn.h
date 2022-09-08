#ifdef HTTPCONNECTION_H
#define HTTPCONNECTION_H

class httpConn{
public:
    static int m_epollFd ;
    static int m_userCount ;

    httpConn();
    ~httpConn();

    void process(); //处理请求
private:
    int curSocketFd;
    sockaddr_in curAddress;
};
#endif