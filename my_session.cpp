//
// Created by lqx on 2021/3/6.
//

#include "my_session.h"
#include <unistd.h>

MySession::MySession(int fd) {
    this->fd = fd;
}

inline int MySession::sessionId() {
    return this->fd;
}

int MySession::write(char *buff, int len) {
    int n = ::write(this->fd, buff, len);
    if (n == -1) {//发送失败
        this->makrWriteFial(buff, len);
    } else {
        delete[] buff;
    }
    return n;
}

int MySession::read(char *buff, int len) {
    return ::read(this->fd, buff, len);
}

int MySession::reWrite() {
    if (this->unsend == nullptr) {
        return 0;
    }
    int n = this->write(this->unsend, this->unsendSize);
    if (n > 0) {
        this->makrRewriteOk();
    }
    return n;
}

void MySession::makrWriteFial(char *buff, int size) {
    this->unsend = buff;
    this->unsendSize = size;
}

void MySession::makrRewriteOk() {
    delete[] this->unsend;
    this->unsend = nullptr;
    this->unsendSize = 0;
}

MySession::~MySession() {
    close(this->fd);
}


