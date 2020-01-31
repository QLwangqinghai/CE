//
//  CCAllocator.c
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCNet.h"

//int CCSocketInfoGetLocal(int fd, CCSocketInfo * _Nonnull info) {
//    
//    
//    return 0;
//}
//
//int CCSocketInfoGetPeer(int fd, CCSocketInfo * _Nonnull info) {
//    
//    
//    
//}

int CCSocketCreate(int addressFamily, int type, int protocol) {
    return socket(addressFamily, type, protocol);
}

int CCSocketBind(int socket, const struct sockaddr * _Nonnull address, socklen_t addressLength) {
    return bind(socket, address, addressLength);
}

int CCSocketListen(int socket, int backlog) {
    return listen(socket, backlog);
}
