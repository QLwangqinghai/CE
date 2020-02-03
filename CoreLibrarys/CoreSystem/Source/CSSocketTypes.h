//
//  CSSocketTypes.h
//  CoreSystem
//
//  Created by wangqinghai on 2017/8/28.
//  Copyright © 2017年 WangQinghai. All rights reserved.
//

#ifndef CSSocketTypes_h
#define CSSocketTypes_h

#include <CoreSystem/CSBase.h>


typedef enum CSIpType {
//    CSIpTypeNone = 0,
    CSIpTypeV4 = 1,
    CSIpTypeV6 = 2,
} CSIpType_e;

typedef enum CSSocketAddressFamily {
    /// AF_INET (IPV4)
    CSSocketAddressFamilyInet = 1,
    
    /// AF_INET6 (IPV6)
    CSSocketAddressFamilyInet6 = 2,
    
    /// AF_UNIX
    CSSocketAddressFamilyUnix = 3,
} CSSocketAddressFamily_e;

static inline CSSocketAddressFamily_e CSSocketProtocolFamilyFromSocklen(socklen_t len) {
    
    socklen_t addrSize4 = sizeof(struct sockaddr_in);
    socklen_t addrSize6 = sizeof(struct sockaddr_in6);
    socklen_t addrSizeu = sizeof(struct sockaddr_un);

    if (len == addrSize4) {
        return CSSocketAddressFamilyInet;
    } else if (len == addrSize6) {
        return CSSocketAddressFamilyInet6;
    } else if (len == addrSizeu) {
        return CSSocketAddressFamilyUnix;
    } else {
        return 0;
    }
}

static inline int CSSocketProtocolFamilyValue(CSSocketAddressFamily_e family) {
    switch (family) {
        case CSSocketAddressFamilyInet:
            return AF_INET;
        case CSSocketAddressFamilyInet6:
            return AF_INET6;
        case CSSocketAddressFamilyUnix:
            return AF_UNIX;
        default:
            return 0;
    }
}

static inline CSSocketAddressFamily_e CSSocketProtocolFamilyGetFamily(int forValue) {
    switch (forValue) {
        case AF_INET:
            return CSSocketAddressFamilyInet;
        case AF_INET6:
            return CSSocketAddressFamilyInet6;
        case AF_UNIX:
            return CSSocketAddressFamilyUnix;
        default:
            return 0;
    }
}

typedef enum CSSocketType {
    CPSocketTypeStream = 1,//SOCK_STREAM 流式socket(TCP)
    CPSocketTypeDatagram = 2,//SOCK_DGRAM 数据报式socket(UDP)
    CPSocketTypeRaw = 3,//SOCK_RAW 原始套接字
    CPSocketTypeReliablyDeliveredMessage = 4,//SOCK_RDM 一种可靠的UDP形式，即保证交付数据报但不保证顺序
    CPSocketTypeSequencedPacket = 5,//SOCK_SEQPACKET 可靠的连续数据包服务
} CSSocketType_e;


static inline int32_t CSSocketTypeValue(CSSocketType_e type) {
    switch (type) {
        case CPSocketTypeStream:
            return SOCK_STREAM;
        case CPSocketTypeDatagram:
            return SOCK_DGRAM;
        case CPSocketTypeRaw:
            return SOCK_RAW;
        case CPSocketTypeReliablyDeliveredMessage:
            return SOCK_RDM;
        case CPSocketTypeSequencedPacket:
            return SOCK_SEQPACKET;
        default:
            return 0;
    }
}

static inline CSSocketType_e CSSocketTypeGetType(int32_t forValue) {
    switch (forValue) {
        case SOCK_STREAM:
            return CPSocketTypeStream;
        case SOCK_DGRAM:
            return CPSocketTypeDatagram;
        case SOCK_RAW:
            return CPSocketTypeRaw;
        case SOCK_RDM:
            return CPSocketTypeReliablyDeliveredMessage;
        case SOCK_SEQPACKET:
            return CPSocketTypeSequencedPacket;
        default:
            return 0;
    }
}

typedef enum CSSocketProtocol {
    CSSocketProtocolTcp = 1,
    CSSocketProtocolUdp = 2,
    CSSocketProtocolUnix = 3,
} CSSocketProtocol_e;

static inline int32_t CPSocketProtocolValue(CSSocketProtocol_e proto) {
    switch (proto) {
        case CSSocketProtocolTcp:
            return IPPROTO_TCP;
        case CSSocketProtocolUdp:
            return IPPROTO_UDP;
        case CSSocketProtocolUnix:
            return 0;
        default:
            return 0;
    }
}

static inline CSSocketProtocol_e CPSocketProtocolGetPorto(int32_t forValue) {
    switch (forValue) {
        case IPPROTO_TCP:
            return CSSocketProtocolTcp;
        case IPPROTO_UDP:
            return CSSocketProtocolUdp;
        case 0:
            return CSSocketProtocolUnix;
        default:
            return 0;
    }
}

typedef enum {
    CPSocketCreateTypeTCPClient = 1,
    CPSocketCreateTypeTCPServer = 2,
    CPSocketCreateTypeAccepted = 3,
    CPSocketCreateTypeLocalClient = 4,
    CPSocketCreateTypeLocalServer = 5,
    CPSocketCreateTypeLocalAccepted = 6,
} CPSocketCreateType_e;

typedef struct {
    uint8_t const createType;
    uint8_t tag;
    uint16_t tag1;//2
    int const sockfd;//4
}CPSocketIdentifier_t;

static inline bool CPSocketIdentifierIsEqual(CPSocketIdentifier_t left, CPSocketIdentifier_t right) {
    return (memcmp(&left, &right, sizeof(CPSocketIdentifier_t)) == 0);
}

static uint32_t CPSocketIdentifierMakeTag = 0;

static inline CPSocketIdentifier_t CPSocketIdentifierMake(uint8_t createType, uint16_t tag1, int sockfd) {
    uint32_t tag = ++CPSocketIdentifierMakeTag;
    CPSocketIdentifier_t si = {createType, (uint8_t)tag, tag1, sockfd};
    return si;
}

typedef struct _CPSocketInfo {
    CPSocketIdentifier_t identifier;
    socklen_t targetAddrLen;
    struct sockaddr * _Nonnull targetAddr;
}CPSocketInfo_t;

typedef struct _CPTcpServerSocketInfo {
    CPSocketInfo_t base;
    int port;
}CPTcpServerSocketInfo_t;

static inline int CPSocketInfoGetSockfd(const CPSocketInfo_t * _Nonnull info) {
    return info->identifier.sockfd;
}

//static inline CPSocketProtocolFamily_e CPSocketInfoGetFamily(const CPSocketInfo_t * _Nonnull info) {
//    return CPSocketIdentifierGetFamily(&(info->identifier));
//}
//static inline CPSocketType_e CPSocketInfoGetType(const CPSocketInfo_t * _Nonnull info) {
//    return CPSocketIdentifierGetType(&(info->identifier));
//}
//static inline CPSocketProtocol_e CPSocketInfoGetProtocol(const CPSocketInfo_t * _Nonnull info) {
//    return CPSocketIdentifierGetProtocol(&(info->identifier));
//}


//CPSocketBlockStatus_e CPSocketInfoGetBlockStatus(const CPSocketInfo_t * _Nonnull info) {
//    return info->blockStatus;
//}

typedef union {
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
    struct sockaddr_un addrPath;
} CPSocketAddress_u;
typedef enum {
    CPAddressTypeIpV4 = 1,
    CPAddressTypeIpV6 = 2,
    CPAddressTypePath = 3,
} CPAddressType_e;

typedef struct CPSocketAddress {
    CPAddressType_e type;
    CPSocketAddress_u addr;
}CPSocketAddress_t;

typedef struct _CPSocketIpInfo {
    int addressType;//CPAddressType_e
    CPSocketAddress_u addr;
}CPSocketIpInfo_t;


typedef enum CPSocketShutdownType {
    CPSocketShutdownTypeRead = 1,
    CPSocketShutdownTypeWrite = 2,
    CPSocketShutdownTypeReadWrite = 3,
} CPSocketShutdownType_e;

typedef enum CPSocketBlockStatus {
    CPSocketBlockStatusBlock = 0,
    CPSocketBlockStatusNoBlock,
} CPSocketBlockStatus_e;


const static int CPSocketIpStringContentMemorySize = (INET6_ADDRSTRLEN + 1) * sizeof(char);
typedef struct _CPSocketIpString {
    char content[INET6_ADDRSTRLEN + 1];
}CPSocketIpString_t;

//
//typedef struct _CPSocketStatisticsHandler {
//    uint32_t tag;
//    int32_t refreceCount;//最多容纳INT32_MAX 个ip 数据
//    void (*didJoin)(void * ipGroup);
//    void (*didRemove)(void * ipGroup);
//    
//}CPSocketStatisticsHandler_t;
//
//
////通过ip 分组
//typedef struct _CPSocketIpStatisticsGroup {//放到一个hash map 中
//    int32_t refreceCount;
//    CPSocketIpInfo_t ipInfo;
//    void (*didJoin)(void * item);
//    void (*didRemove)(void * item);
//    
//}CPSocketIpStatisticsGroup_t;
//typedef CPSocketIpStatisticsGroup_t * CPSocketIpStatisticsGroupRef;
//
////typedef enum CPSocketLogItemFlag {
////    CPSocketLogItemFlagRead,
////    CPSocketLogItemFlagWrite,
////    CPSocketLogItemFlagConnect,
////    CPSocketLogItemFlagClose,
////    CPSocketLogItemFlagHandshakeError,
////    CPSocketLogItemFlagHandshakeDenied,
////
////}CPSocketLogItemFlag_e;
//
////128 字节
//typedef struct _CPSocketStatisticsItemInfo {
//    CPSocketIpStatisticsGroup_t * groupRef;//所在ip 组
//    struct _CPSocketStatisticsItemInfo * last;
//    struct _CPSocketStatisticsItemInfo * next;
//    uint64_t sequence;
//    uint64_t time;
//    int32_t sockfd;
//    uint32_t flag;
//    uint8_t content[80];
//}CPSocketStatisticsItemInfo_t;
//
//typedef CPSocketStatisticsItemInfo_t * CPSocketStatisticsItemInfoRef;
//
//
//typedef struct _CPSocketStatisticsItemList {
//    CPSocketStatisticsItemInfoRef first;
//    CPSocketStatisticsItemInfoRef latest;
//    uint64_t count;
//}CPSocketStatisticsItemList_t;
//typedef CPSocketStatisticsItemList_t * CPSocketStatisticsItemListRef;
//

#endif /* CPSocketTypes_h */
