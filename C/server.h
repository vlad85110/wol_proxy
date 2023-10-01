//
// Created by Влад Кирилов on 28.09.2023.
//

#ifndef WOL_PROXY_SERVER_H
#define WOL_PROXY_SERVER_H


#include <sys/types.h>

typedef struct MagicPacket {
    char payload[102];
} MagicPacket;

typedef struct Server {

    int port;
    char *send_address;

    int listen_socket;
    int send_socket;

    fd_set read_set;
    fd_set write_set;

    char receive_buffer[1024];

    char *mac_address;

    MagicPacket magic_packet;
} Server;

Server * create_server(int port, const char *sendAddress, const char *macAddress);
int run_proxy(Server* server);
int send_wol(Server* server);
void delete_server(Server* server);


#endif //WOL_PROXY_SERVER_H
