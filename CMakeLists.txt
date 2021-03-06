cmake_minimum_required(VERSION 3.13.4)
project(epoll2)

set(CMAKE_CXX_STANDARD 11)

add_executable(epoll2
        main.cpp
        epoll_server.cpp
        epoll_server.h
        session.h
        my_session.cpp
        my_session.h)
