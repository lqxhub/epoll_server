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
    const int port;//端口
    map<int, Session *> sessions;
    int socketFd{};

    const int eventNum;
    const int eTimeout;
public:
    EpollServer() : EpollServer(8088, 20, -1) {}

    EpollServer(int _port, int _eventNum, int _eTimeout) : port(_port), eventNum(_eventNum), eTimeout(_eTimeout) {}

    bool run();

    void addSession(Session *session);

    void delSession(Session *session);

    Session *getSession(int sessionId);

    virtual ~EpollServer();

private:
    bool createSocket();

    epoll_event *events{};

    int eFd{};
};


#endif //EPOLL2_EPOLL_SERVER_H
