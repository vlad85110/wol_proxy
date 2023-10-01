//
// Created by Влад Кирилов on 27.09.2023.
//

//#include "server/Server.h"
#include "C/server.h"
//#include <iostream>

int main() {
//    Server server(24242, "192.168.0.255", "00:D8:61:DA:99:F9");
//    try {
//        server.run_proxy();
//    } catch (const std::runtime_error &error) {
//        std::cerr << error.what() << std::endl;
//    }
//
//    server.send_wol();

    const char *ip_address = "192.168.0.255";
    const char *mac_address = "00:D8:61:DA:99:F9";

    Server *server = create_server(24242, ip_address, mac_address);

    if (!server) {
        return -1;
    }

    run_proxy(server);


    return 0;
}