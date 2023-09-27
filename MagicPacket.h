//
// Created by Влад Кирилов on 27.09.2023.
//

#ifndef WOL_SERVER_MAGICPACKET_H
#define WOL_SERVER_MAGICPACKET_H

#include "types.h"

struct MagicPacket {
    std::unique_ptr<char[]> payload;
    explicit MagicPacket(const String& mac_address);

    [[nodiscard]] const char * get_payload() const;
};


#endif //WOL_SERVER_MAGICPACKET_H
