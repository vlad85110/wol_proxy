//
// Created by Влад Кирилов on 27.09.2023.
//

#include "Server.h"
#include <iostream>

int main() {
    Server server(24242, "192.168.0.255", "00:D8:61:DA:99:F9");

    try {
        server.run();
    } catch (std::runtime_error &error) {
        std::cerr << error.what() << std::endl;
    }

    return 0;
}