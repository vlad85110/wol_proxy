//
//  send_wol.h
//  ServerWol
//
//  Created by Влад Кирилов on 27.09.2023.
//

#ifndef send_wol_h
#define send_wol_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int wake_on_lan(const char * ip);

#endif /* send_wol_h */
