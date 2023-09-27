cmake_minimum_required(VERSION 3.26)
project(wol_proxy)

set(CMAKE_CXX_STANDARD 17)

add_executable(wol_proxy main.cpp
        Server.cpp
        send_wol.c
        MagicPacket.cpp)
