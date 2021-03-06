//
// Created by lqx on 2021/3/6.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "epoll_server.h"
#include "my_session.h"
#include <sys/epoll.h>
#include <cstdlib>

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
    epev.events = EPOLLIN | EPOLLET;
    epev.data.fd = this->socketFd;
    epoll_ctl(eFd, EPOLL_CTL_ADD, this->socketFd, &epev);
    this->events = new epoll_event[this->eventNum];

    char buff[1024];
    while (true) {
        int eNum = epoll_wait(eFd, this->events, this->eventNum, this->eTimeout);

        if (eNum == -1) {
            cout << "epoll_wait" << endl;
            return false;
        }
        for (int i = 0; i < eNum; i++) {
            if (this->events[i].data.fd == this->socketFd) {//上线
                if (this->events[i].events & EPOLLIN) {
                    sockaddr_in cli_addr{};
                    socklen_t length = sizeof(cli_addr);
                    int fd = accept(this->socketFd, (sockaddr *) &cli_addr, &length);
                    if (fd > 0) {
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
                if (this->events[i].events & EPOLLIN) {
                    int len = session->read(buff, 1024);
                    if (len == -1) {
                        this->delSession(session);
                        cout << "client out fd:" << this->events[i].data.fd << endl;
                    } else {
                        cout << "read session: " << session->sessionId() << "  " << buff << endl;
                        string sid = "hello: " + to_string(session->sessionId());

                        char *t = new char[sid.length()];

                        for (int i = 0; i < sid.length(); i++) {
                            t[i] = sid[i];
                        }

                        session->write(t, sid.length());
                    }
                } else if (this->events[i].events & EPOLLERR || this->events[i].events & EPOLLHUP) {
                    this->delSession(session);
                }
            }
        }
    }
}

void EpollServer::addSession(Session *session) {
    epoll_event epev{};
    epev.events = EPOLLIN;
    epev.data.fd = session->sessionId();
    epoll_ctl(this->eFd, EPOLL_CTL_ADD, session->sessionId(), &epev);
    this->sessions[session->sessionId()] = session;
}

void EpollServer::delSession(Session *session) {
    epoll_ctl(this->eFd, EPOLL_CTL_DEL, session->sessionId(), nullptr);
    this->sessions.erase(session->sessionId());
    delete session;
}

Session *EpollServer::getSession(int sessionId) {
    return this->sessions[sessionId];
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
