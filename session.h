//
// Created by lqx on 2021/3/6.
//

#ifndef EPOLL2_SESSION_H
#define EPOLL2_SESSION_H

class Session {
public:
    //获取这个session的id
    virtual int sessionId() = 0;

    //从socket中读取内容
    virtual int read(char *buff, int len) = 0;

    //向socket中写
    virtual int write(char *buff, int len) = 0;

    //写失败后重写
    virtual int reWrite() = 0;

    virtual ~Session() = default;

};

#endif //EPOLL2_SESSION_H
