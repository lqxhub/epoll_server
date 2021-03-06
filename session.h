//
// Created by lqx on 2021/3/6.
//

#ifndef EPOLL2_SESSION_H
#define EPOLL2_SESSION_H

class Session {
public:
    virtual int sessionId() = 0;

    virtual int read(char *buff ,int len) = 0;

    virtual int write(char *buff,int len) = 0;

    virtual ~Session()= default;

};

#endif //EPOLL2_SESSION_H
