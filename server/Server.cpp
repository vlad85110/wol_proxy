//
// Created by Влад Кирилов on 27.09.2023.
//

#include "Server.h"

#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/select.h>
#include <unistd.h>
#include "MagicPacket.h"
#include <iostream>
#include <sys/fcntl.h>

using std::strerror;
#define Errno String(strerror(errno))

namespace {
    class SyscallException : public std::runtime_error {
    public:
        explicit SyscallException(const std::string &string) : runtime_error(string + " : " + Errno) {}
    };
}

Server::Server(int port, String sendAddress, const String &macAddress)
        : port(port), send_address(std::move(sendAddress)),
          magic_packet(MagicPacket(macAddress)),
          mac_address(macAddress), read_set(), write_set() {
    receive_buffer = std::make_unique<char[]>(1024);

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
}

void Server::run() {
    create_listen_socket();
    create_send_socket();

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_SET(listen_socket, &read_set);

    fd_set read_res, write_res;

    //TODO придумать как его остановить
    while (true) {
        read_res = read_set;
        write_res = write_set;

        auto ret_val = select(FD_SETSIZE, &read_res, &write_res, nullptr, nullptr);

        if (ret_val < 0) {
            throw SyscallException("select error");
        }

        if (FD_ISSET(listen_socket, &read_res)) {
            while (true) {
                auto bytesRead = recv(listen_socket, receive_buffer.get(), 1024, 0);

                if (bytesRead < 0) {
                    break;
                }

                if (bytesRead == 0) {}//todo error handle
            }

            FD_SET(send_socket, &write_set);
        }

        if (FD_ISSET(send_socket, &write_res)) {
            std::cerr << 1 ;
            FD_CLR(send_socket, &write_set);
            send_wol();
        }
    }
}

void Server::send_wol() const {
    struct sockaddr_in address{};
    address.sin_addr.s_addr = inet_addr(send_address.c_str());
    address.sin_port = htons(9);
    address.sin_family = AF_INET;

    auto res = sendto(send_socket, magic_packet.get_payload(), 102, 0,
                      (const struct sockaddr *) &address, sizeof(address));

    if (res == -1) {
        throw SyscallException("unable to send wol packet");
    }
}

void Server::create_listen_socket() {
    struct sockaddr_in server_address{};

    listen_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (listen_socket < 0) {
        throw SyscallException("unable to create socket");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    auto res = bind(listen_socket,
                    (const struct sockaddr *) &server_address,
                    sizeof(server_address));

    if (res < 0) {
        throw SyscallException("unable to bind socket");
    }

    if (fcntl(listen_socket, F_SETFL, O_NONBLOCK) == -1) {
        throw SyscallException("unable to set socket nonblock");
    }
}

void Server::create_send_socket() {
    send_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (send_socket < 0) {
        throw SyscallException("unable to create socket");
    }

    int yes = 1;
    int ret = setsockopt(send_socket, SOL_SOCKET, SO_BROADCAST, (char *) &yes, sizeof(int));

    if (ret == -1) {
        throw SyscallException("unable to set socket broadcast option");
    }

    if (fcntl(send_socket, F_SETFL, O_NONBLOCK) == -1) {
        throw SyscallException("unable to set socket nonblock");
    }
}

Server::~Server() {
    close(listen_socket);
    close(send_socket);

}
