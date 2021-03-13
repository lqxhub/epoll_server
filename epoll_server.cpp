//
// Created by lqx on 2021/3/6.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "epoll_server.h"
#include "my_session.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <memory>

using namespace std;

bool EpollServer::run() {
    if (!this->createSocket()) {
        return false;
    }
    int efd = epoll_create(1);
    if (efd == -1) {
        cout << "create epoll fail" << endl;
        return false;
    }
    this->eFd = efd;

    epoll_event epev{};
    epev.events = EPOLLIN;
    epev.data.fd = this->socketFd;
    epoll_ctl(eFd, EPOLL_CTL_ADD, this->socketFd, &epev);
    this->events = new epoll_event[this->eventNum];

    unique_ptr<char> buff(new char[this->buffLen]);

    while (true) {
        int eNum = epoll_wait(eFd, this->events, this->eventNum, this->eTimeout);

        if (eNum == -1) {
            cout << "epoll_wait" << endl;
            return false;
        }
        for (int i = 0; i < eNum; i++) {
            if (this->events[i].data.fd == this->socketFd) {//新连接
                if (this->events[i].events & EPOLLIN) {
                    sockaddr_in cli_addr{};
                    socklen_t length = sizeof(cli_addr);
                    int fd = accept(this->socketFd, (sockaddr *) &cli_addr, &length);
                    if (fd > 0) {
                        if (!this->block) {//如果是非阻塞模式
                            int f = fcntl(fd, F_GETFL, 0);
                            if (f < 0) {
                                cout << "set no block error, fd:" << fd << endl;
                                continue;
                            }
                            if (fcntl(fd, F_SETFL, f | O_NONBLOCK) < 0) {
                                cout << "set no block error, fd:" << fd << endl;
                                continue;
                            }
                        }
                        cout << "client on line fd:" << fd << endl;
                        Session *ms = new MySession(fd);
                        this->addSession(ms);
                    }
                }
            } else {
                Session *session = this->getSession(this->events[i].data.fd);
                if (session == nullptr) {
                    cout << "get session not find, id:" << this->events[i].data.fd << endl;
                    continue;
                }

                if (this->events[i].events & EPOLLERR || this->events[i].events & EPOLLHUP) {//出错或者断开
                    this->delSession(session);
                } else {
                    if (this->events[i].events & EPOLLOUT) {//可写
                        int n = session->reWrite();
                        if (n > 0) {
                            epoll_event _epev{};
                            _epev.events = EPOLLIN;
                            _epev.data.fd = this->events[i].data.fd;
                            epoll_ctl(this->eFd, EPOLL_CTL_MOD, session->sessionId(), &_epev);
                        }
                    }
                    if (this->events[i].events & EPOLLIN) {
                        int len = session->read(buff.get(), this->buffLen);
                        if (len == -1) {
                            this->delSession(session);
                            cout << "client out fd:" << this->events[i].data.fd << endl;
                        } else {
                            cout << "read session: " << session->sessionId() << "  " << buff.get() << endl;
                            string sid = "hello: " + to_string(session->sessionId());
                            char *t = new char[sid.length()];
                            sid.copy(t, sid.length(), 0);
                            this->writeToSession(session, t, sid.length());
                        }
                    }
                }
            }
        }
    }
}

void EpollServer::addSession(Session *session) {
    epoll_event epev{};
    epev.events = EPOLLIN;
    if (!this->block) {//是非阻塞
        epev.events |= EPOLLET;
    }
    epev.data.fd = session->sessionId();
    epoll_ctl(this->eFd, EPOLL_CTL_ADD, session->sessionId(), &epev);
    this->sessions[session->sessionId()] = session;
}

void EpollServer::delSession(Session *session) {
    cout << "session: " << session->sessionId() << "out" << endl;
    epoll_ctl(this->eFd, EPOLL_CTL_DEL, session->sessionId(), nullptr);
    this->sessions.erase(session->sessionId());
    delete session;
}

Session *EpollServer::getSession(int sessionId) {
    return this->sessions[sessionId];
}


void EpollServer::writeToSession(Session *session, char *data, int size) {
    int n = session->write(data, size);
    if (n == -1) {//写出错.注册可写事件,等待可写时重写
        epoll_event _epev{};
        _epev.events = EPOLLIN | EPOLLOUT;
        _epev.data.fd = session->sessionId();
        epoll_ctl(this->eFd, EPOLL_CTL_MOD, session->sessionId(), &_epev);
    }
}

bool EpollServer::createSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        cout << "create socket error" << endl;
        return false;
    }
    this->socketFd = fd;

    sockaddr_in sockAddr{};
    sockAddr.sin_port = htons(this->port);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if (bind(socketFd, (sockaddr *) &sockAddr, sizeof(sockAddr)) == -1) {
        cout << "bind error" << endl;
        return false;
    }

    if (listen(socketFd, 100) == -1) {
        cout << "listen error" << endl;
        return false;
    }
    return true;
}

EpollServer::~EpollServer() {
    for (auto &session : this->sessions) {
        delete (session.second);
    }
    this->sessions.clear();

    delete[] this->events;
}
