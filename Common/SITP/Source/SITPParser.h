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

typedef CCInt SITPParserCode;

typedef struct {
    SITPParserCode code;
    const char * _Nonnull message;
} SITPParserError_t;

extern CCUInt8 const SITPTypeCodeBoolTrue;
extern CCUInt8 const SITPTypeCodeBoolFalse;

extern SITPParserCode const SITPParserCodeUnknownError;
extern SITPParserCode const SITPParserCodeNeedMoreData;
extern SITPParserCode const SITPParserCodeParamError;
extern SITPParserCode const SITPParserCodePaddingError;
extern SITPParserCode const SITPParserCodeUnknownDataSubtype;
extern SITPParserCode const SITPParserCodeContentLengthControlError;
extern SITPParserCode const SITPParserCodeContentLengthError;
extern SITPParserCode const SITPParserCodeIndexCountError;
extern SITPParserCode const SITPParserCodeUnknownMessageEncodeError;
extern SITPParserCode const SITPParserCodeUnknownTypeError;

typedef const SITPParserError_t * SITPParserErrorRef;

typedef struct {
    uint8_t * _Nonnull content;
    SITPByteRange range;
} SITPByteBuffer_t;

typedef struct {
    SITPField_t * _Nullable fields;
    SITPIndex erroredFieldIndex;
    SITPIndex fieldCount;
    SITPSize readLength;
} SITPParserParseMessageResult_t;


typedef enum {
    //field[type(1Byte), optional(contentLength, 4Byte)]
    SITPMessageEncodingNormal = 0x0,
    
    //field[type(5bit), optional(contentLengthByteCount, 3bit), optional(contentLength, contentLengthByteCount Byte)]
    SITPMessageEncodingCompressFieldContentLength = 0x1,
    
    
    SITPMessageEncodingError = INT32_MAX,
} SITPMessageEncoding_e;

//encoding 是否是无效的
static inline CCBool SITPMessageEncodingIsInvalid(SITPMessageEncoding_e encoding) {
    return encoding != SITPMessageEncodingNormal && encoding != SITPMessageEncodingCompressFieldContentLength;
};


//typedef struct {
//    SITPByteRange range;
//    SITPByteBuffer_t * _Nonnull buffers;
//    uint32_t bufferCount;
//    uint32_t lastBufferIndex;
//} SITPParserDataContext_t;


//if SITPParserParseMessageResult_t.fields != NULL, must call CCDealloc();
SITPParserParseMessageResult_t SITPParserParseMessage(SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserErrorRef _Nullable * _Nonnull errorRef);


typedef SITPParserCode (*SITPParserParseCallback_f)(void * _Nullable context, SITPField_t field);



SITPParserCode SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback);

//SITPParserCode SITPParserReadFinishField(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteSize length);









#endif /* SITPParser_h */
