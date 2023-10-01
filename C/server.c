//
// Created by Влад Кирилов on 28.09.2023.
//

#include "server.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MALLOC_ERROR (-2)
#define SYSCALL_ERROR (-1)
#define SUCCESS (0)
#define nullptr NULL

int create_send_socket(Server *server);

int create_listen_socket(Server *server);

int init_magic_packet(Server *server);

Server * create_server(int port, const char *sendAddress, const char *macAddress) {
    Server* server = (Server *) malloc(sizeof (Server));

    if (!server) {
        return nullptr;
    }

    server->port = port;

    server->send_address = malloc((strlen(sendAddress) + 1));
    server->mac_address = malloc((strlen(macAddress) + 1));

    strcpy(server->send_address, sendAddress);
    strcpy(server->mac_address, macAddress);

    if (!server->send_address || !server->mac_address) {
        return nullptr;
    }

    int res = create_send_socket(server);
    if (res < 0) {
        return nullptr;
    }

    res = create_listen_socket(server);
    if (res < 0) {
        return nullptr;
    }

    init_magic_packet(server);

    FD_ZERO(&server->read_set);
    FD_ZERO(&server->write_set);

    return server;
}

int run_proxy(Server *server) {
    FD_SET(server->listen_socket, &server->read_set);

    fd_set read_res, write_res;

    //TODO придумать как его остановить
    while (true) {
        read_res = server->read_set;
        write_res = server->write_set;

        int ret_val = select(FD_SETSIZE, &read_res, &write_res, nullptr, nullptr);

        if (ret_val < 0) {
            return SYSCALL_ERROR;
        }

        if (FD_ISSET(server->listen_socket, &read_res)) {
            fprintf(stderr, "1");

            while (true) {
                size_t bytesRead = recv(server->listen_socket, server->receive_buffer, 1024, 0);

                if (bytesRead == -1) {
                    break;
                }

                if (bytesRead == 0) {}//todo error handle
            }

            FD_SET(server->send_socket, &server->write_set);
        }

        if (FD_ISSET(server->send_socket, &write_res)) {
            FD_CLR(server->send_socket, &server->write_set);
            send_wol(server);
        }
    }
}

int send_wol(Server *server) {
    return 0;
}

void delete_server(Server *server) {
    close(server->listen_socket);
    close(server->send_socket);
    free(server->mac_address);
    free(server->send_address);
}

int create_listen_socket(Server *server) {
    struct sockaddr_in server_address;

    server->listen_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (server->listen_socket < 0) {
        return SYSCALL_ERROR;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server->port);

    int res = bind(server->listen_socket,
                   (const struct sockaddr *) &server_address,
                   sizeof(server_address));

    if (res < 0) {
        return SYSCALL_ERROR;
    }

    if (fcntl(server->listen_socket, F_SETFL, O_NONBLOCK) == -1) {
        return SYSCALL_ERROR;
    }

    return 0;
}

int create_send_socket(Server *server) {
    server->send_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (server->send_socket < 0) {
        return SYSCALL_ERROR;
    }

    int yes = 1;
    int ret = setsockopt(server->send_socket, SOL_SOCKET, SO_BROADCAST, (char *) &yes, sizeof(int));

    if (ret == -1) {
        return SYSCALL_ERROR;
    }

    if (fcntl(server->send_socket, F_SETFL, O_NONBLOCK) == -1) {
        return SYSCALL_ERROR;
    }

    return SUCCESS;
}

int init_magic_packet(Server *server) {
    unsigned char mac[6];

    sscanf(server->mac_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1],
           &mac[2], &mac[3], &mac[4], &mac[5]);

    unsigned char header[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(server->magic_packet.payload, header, sizeof(unsigned char) * 6);
    for (int i = 1; i <= 16; ++i) {
        memcpy(server->magic_packet.payload + i * 6, mac, sizeof(unsigned char) * 6);
    }

    return SUCCESS;
}
