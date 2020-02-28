//
//  FileServiceBase.h
//  FileService
//
//  Created by vector on 2020/2/28.
//

#ifndef FileServiceBase_h
#define FileServiceBase_h

#include <stdio.h>

typedef struct {
    uint8_t dgst[64];
    uint64_t length;
} FileIdentifier;

#endif /* FileServiceBase_h */
