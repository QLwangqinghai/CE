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
int CCSocketAccept(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength) {
    return accept(socket, address, addressLength);
}

int CCSocketGetAddress(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength) {
    return getsockname(socket, address, addressLength);
}

int CCSocketGetPeerAddress(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength) {
    return getpeername(socket, address, addressLength);
}
