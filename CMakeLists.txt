cmake_minimum_required(VERSION 3.26)
project(wol_proxy)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp
        server/Server.cpp
        server/MagicPacket.cpp)

add_executable(wol_proxy ${SOURCE_FILES}
        C/server.h
        C/server.c)
