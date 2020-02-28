//
//  FileManagerBase.h
//  FileManager
//
//  Created by vector on 2019/7/18.
//  Copyright © 2019 angfung. All rights reserved.
//


#include <CCFoundation/CCFoundation.h>
//
//typedef struct _CFMPixelSize {
//    uint32_t width;
//    uint32_t height;
//} CFMPixelSize_s;
//
//typedef struct _CFMByte64 {
//    uint8_t content[64];
//} CFMByte64_s;
//
//typedef struct _CFMByte32 {
//    uint8_t content[32];
//} CFMByte32_s;
//
//
//#ifdef __APPLE__
//#include <libkern/OSByteOrder.h>
//#elif __linux__
//#include <endian.h>
//#elif defined(__FreeBSD__)
//#include <sys/endian.h>
//#elif defined(_WIN32)
//#endif
//
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//
//typedef struct _CFileLimit {
//    uint64_t id: 48;
//    uint64_t role: 4;
//    uint64_t zeroPadding: 10;
//    uint64_t readable: 1;
//    uint64_t writable: 1;
//} CFileLimit_s;
//
//typedef struct _CUserIdentifier {
//    uint64_t id: 48;
//    uint64_t role: 4;
//    uint64_t zeroPadding: 12;
//} CUserIdentifier_s;
//
//#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
//
//typedef struct _CFileLimit {
//    uint64_t writable: 1;
//    uint64_t readable: 1;
//    uint64_t zeroPadding: 10;
//    uint64_t role: 4;
//    uint64_t id: 48;
//} CFileLimit_s;
//typedef struct _CUserIdentifier {
//    uint64_t zeroPadding: 12;
//    uint64_t role: 4;
//    uint64_t id: 48;
//} CUserIdentifier_s;
//
//#else
//#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
//#endif
//
//static inline _Bool CUserIdentifierMake(CUserIdentifier_s * _Nonnull identifier, uint64_t uid, uint8_t role) {
//    if (uid >= 0x1000000000000ULL) {
//        return false;
//    }
//    if (role >= 0xf) {
//        return false;
//    }
//    uint64_t a = 1;
//    
//    CUserIdentifier_s id = {};
//    *((uint64_t *)(&id)) = a;
//    
//}
//
//static inline void aa() {
//    uint64_t a = 1;
//    
//    CUserIdentifier_s id = {};
//    *((uint64_t *)(&id)) = a;
//    
//    memset(&id, 0, sizeof(CUserIdentifier_s));
//    id.role = 1;
//    id.id = 1;
//    a = *((uint64_t *)(&id));
//    printf("a: %llx\n", a);
//
//    memset(&id, 0, sizeof(CUserIdentifier_s));
//    id.role = 32;
//    id.id = 1;
//    a = *((uint64_t *)(&id));
//    printf("a: %llx\n", a);
//
//    
//    printf("aa");
//}
//
