//
// Created by lqx on 2021/3/6.
//

#ifndef EPOLL2_MY_SESSION_H
#define EPOLL2_MY_SESSION_H

#include "session.h"

class MySession : public Session {
public:
    explicit MySession(int fd);

    inline int sessionId() override;

    int read(char *buff, int len) override;

    int write(char *buff, int len) override;

    ~MySession() override;

private:
    int fd;
};


#endif //EPOLL2_MY_SESSION_H
