//
// Created by lqx on 2021/3/6.
//

#include "my_session.h"
#include <unistd.h>
#include <sys/socket.h>

MySession::MySession(int fd) {
    this->fd = fd;
}

inline int MySession::sessionId() {
    return this->fd;
}

int MySession::write(char *buff, int len) {
    return ::write(this->fd, buff, len);
}

int MySession::read(char *buff, int len) {
    return recv(this->fd, buff, len, 0);
}

MySession::~MySession() {
    close(this->fd);
}


