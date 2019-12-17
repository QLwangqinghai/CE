//
//  SITPParser.h
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef SITPParser_h
#define SITPParser_h

#include "SITPType.h"

struct _SITPParser;
typedef struct _SITPParser SITPParser_t;
typedef SITPParser_t * SITPParserPtr;

typedef uint32_t SITPParserCode;

typedef struct {
    uint8_t * _Nonnull content;
    SITPByteSize location;
    SITPByteSize length;
} SITPByteBuffer_t;

typedef SITPParserCode (*SITPParserRead_f)(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);

typedef struct {
    uint32_t control;
    SITPByteSize length;
} SITPParserFieldControl_t;

struct _SITPParser {
    SITPIndexRange index;
    SITPByteSize lengthByteCount;
    SITPByteSize location;
    SITPByteSize length;
    
    uint32_t stackSize;
    uint32_t fieldCount;
    SITPParserFieldControl_t controlStack[20];
    
    SITPField_t fields[16];
    SITPField_t readingField;
};
struct _SITPParserContext {
    SITPIndexRange index;
    SITPByteSize lengthByteCount;
    SITPByteSize location;
    SITPByteSize length;
    
    uint32_t stackSize;
    uint32_t fieldCount;
    SITPParserFieldControl_t controlStack[20];
    
    SITPField_t fields[16];
    SITPField_t readingField;
};


typedef enum {
    SITPParserControlCodeReadFieldHead = 0x1,
    SITPParserControlCodeReadFieldSubtype = 0x2,
    SITPParserControlCodeReadFieldContentLength = 0x3,
    SITPParserControlCodeFieldContent = 0x4,
} SITPParserControlCode_e;

SITPParserCode SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);

SITPParserCode SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);

SITPParserCode SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);

SITPParserCode SITPParserReadFieldContent(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);













#endif /* SITPParser_h */
