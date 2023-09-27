//
// Created by Влад Кирилов on 27.09.2023.
//

#ifndef WOL_SERVER_SERVER_H
#define WOL_SERVER_SERVER_H

#include <locale>
#include "../types.h"
#include "MagicPacket.h"

class Server {
private:
    int port;
    String send_address;

    int listen_socket = 0;
    int send_socket = 0;

    void create_listen_socket();
    void create_send_socket();

    fd_set read_set;
    fd_set write_set;

    std::unique_ptr<char[]> receive_buffer;

    void send_wol() const;

    String mac_address;

    MagicPacket magic_packet;

public:
    Server(int port, String sendAddress, const String& macAddress);

    void run();

    ~Server();
};


#endif //WOL_SERVER_SERVER_H
