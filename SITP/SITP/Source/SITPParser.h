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

extern SITPParserCode const SITPParserCodeSuccess;
extern SITPParserCode const SITPParserCodeNeedMoreData;
extern SITPParserCode const SITPParserCodeParamError;
extern SITPParserCode const SITPParserCodeReadError;

extern SITPParserCode const SITPParserCodeUnknownDataSubType;
extern SITPParserCode const SITPParserCodePaddingError;
extern SITPParserCode const SITPParserCodeMessageSubTypeError;

extern SITPParserCode const SITPParserCodeLengthByteCountError;


extern SITPParserCode const SITPParserCodeUnknownError;


typedef struct {
    uint8_t * _Nonnull content;
    SITPByteRange range;
} SITPByteBuffer_t;


typedef SITPParserCode (*SITPByteReader_f)(void * _Nonnull context, void * _Nonnull buffer, SITPByteRange range);

typedef struct {
    void * _Nonnull context;
    SITPByteReader_f _Nonnull read;
} SITPByteReader_t;

typedef struct {
    SITPByteRange range;
    SITPByteBuffer_t * _Nonnull buffers;
    uint32_t bufferCount;
    uint32_t lastReadBufferIndex;
} SITPByteReaderMemoryContext_t;

typedef SITPParserCode (*SITPParserRead_f)(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

typedef struct {
    SITPParserRead_f _Nonnull func;
    SITPByteSize length;
} SITPParserFieldControl_t;

struct _SITPParser {
    SITPIndexRange readingIndexs;
    SITPByteSize lengthByteCount;
    SITPByteRange byteRange;
    
    //读取的数据长度
    SITPByteSize readLength;

    uint32_t controlOffset: 16;
    uint32_t controlCount: 16;
    SITPParserFieldControl_t controls[8];
    
    SITPField_t readingField;
    
    uint32_t tmp;
};
//typedef struct {
//    SITPByteRange range;
//    SITPByteBuffer_t * _Nonnull buffers;
//    uint32_t bufferCount;
//    uint32_t lastBufferIndex;
//} SITPParserDataContext_t;


typedef enum {
    SITPParserControlCodeReadFieldHead = 0x1,
    SITPParserControlCodeReadFieldSubtype = 0x2,
    SITPParserControlCodeReadFieldContentLength = 0x3,
    SITPParserControlCodeFieldContent = 0x4,
} SITPParserControlCode_e;

typedef SITPParserCode (*SITPParserParseCallback_f)(void * _Nullable context, SITPField_t field);


SITPParserCode SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback);




//SITPParserCode SITPParserReadFinishField(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);









#endif /* SITPParser_h */
