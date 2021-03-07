//
// Created by lqx on 2021/3/6.
//

#ifndef EPOLL2_EPOLL_SERVER_H
#define EPOLL2_EPOLL_SERVER_H

#include <map>
#include <sys/epoll.h>
#include "session.h"

using namespace std;

class EpollServer {
private:
    //端口
    const int port;

    map<int, Session *> sessions;
    int socketFd{};

    const int eventNum;
    const int eTimeout;

    epoll_event *events{};

    int eFd = 0;

    bool block = true;

public:
    EpollServer() : EpollServer(8088, 20, -1, true) {}

    EpollServer(int _port, int _eventNum, int _eTimeout, bool _block) : port(_port), eventNum(_eventNum),
                                                                        eTimeout(_eTimeout) {
        this->block = _block;
    }

    //运行server
    bool run();

    //新连接加入
    void addSession(Session *session);

    //删除连接
    void delSession(Session *session);

    //获一个session
    Session *getSession(int sessionId);

    //向一个session中写数据
    void writeToSession(Session *session, char *data, int size);

    virtual ~EpollServer();

private:
    bool createSocket();
};


#endif //EPOLL2_EPOLL_SERVER_H
