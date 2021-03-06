#include <iostream>
#include "epoll_server.h"

int main() {
    auto *server = new EpollServer();
    server->run();
}
