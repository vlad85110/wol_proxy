//
//  send_wol.c
//  ServerWol
//
//  Created by Влад Кирилов on 27.09.2023.
//

#include "send_wol.h"

void create_magic_packet(const char* mac_address, char* magic_packet) {
    unsigned char mac[6];

    sscanf(mac_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1],
           &mac[2], &mac[3], &mac[4], &mac[5]);

    unsigned char header[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    memcpy(magic_packet, header, sizeof(unsigned char) * 6);
    for (int i = 1; i <= 16; ++i) {
        memcpy(magic_packet + i * 6, mac, sizeof (unsigned char) * 6);
    }
}

int wake_on_lan(const char * ip) {
    size_t port = 9;
    in_addr_t addr = inet_addr(ip);
    const char* mac_address = "00:D8:61:DA:99:F9";
    char magic_packet[102];
    create_magic_packet(mac_address, magic_packet);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }
    
    int yes = 1;
    int ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(int));
    if (ret == -1) {
        return -1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_addr.s_addr = addr;
    servaddr.sin_port = htons(port);
    servaddr.sin_family = AF_INET;

    int res = sendto(sock_fd, magic_packet, 102, 0,
           (const struct sockaddr *) &servaddr, sizeof(servaddr));

    close(sock_fd);
    return res == -1 ? res : 0;
}
