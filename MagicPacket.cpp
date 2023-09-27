//
// Created by Влад Кирилов on 27.09.2023.
//

#include "MagicPacket.h"
#include <cstdio>

MagicPacket::MagicPacket(const String& mac_address) {
    payload = std::make_unique<char []>(102);

    unsigned char mac[6];
    sscanf(mac_address.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1],
           &mac[2], &mac[3], &mac[4], &mac[5]);

    unsigned char header[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(payload.get(), header, sizeof(unsigned char) * 6);
    for (int i = 1; i <= 16; ++i) {
        memcpy(payload.get() + i * 6, mac, sizeof (unsigned char) * 6);
    }
}

const char *MagicPacket::get_payload() const {
    return payload.get();
}
