//
//  SITPParser.c
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "SITPParser.h"
#include <assert.h>
#include "SITPIndexShift.h"

SITPParserCode const SITPParserCodeSuccess = 0;
SITPParserCode const SITPParserCodeNeedMoreData = 1;
SITPParserCode const SITPParserCodeParamError = 2;
SITPParserCode const SITPParserCodeReadError = 3;

SITPParserCode const SITPParserCodeUnknownError = UINT32_MAX;


SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader);




//SITPParserCode SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}

SITPParserCode SITPByteReaderMemoryRead(void * _Nonnull context, uint8_t * _Nonnull buffer, SITPByteRange range) {
    assert(context);
    SITPByteReaderMemoryContext_t * memoryContext = context;
    
    return SITPParserCodeSuccess;
}



void SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback) {
    if (range.length == 0) {
        return;
    }
    SITPByteReaderMemoryContext_t memoryContext = {};
    memoryContext.range = range;
    memoryContext.buffers = buffers;
    memoryContext.bufferCount = bufferCount;
    memoryContext.lastReadBufferIndex = 0;
    
    SITPByteReader_t reader = {
        .context = &memoryContext,
        .read = SITPByteReaderMemoryRead,
    };
    
    SITPParser_t parser = {};
    parser.byteRange = range;
    
    
    
    
}

SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader) {
    assert(parser);
    SITPParserCode result = SITPParserCodeSuccess;
    uint8_t byte = 0;
    SITPByteRange range = {
        .location = parser->readLength + parser->byteRange.location,
        .length = 1,
    };
    result = reader.read(reader.context, &byte, range);
    if (SITPParserCodeSuccess == result) {
        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
        parser->readingIndexs.location += shift.offset;
        parser->readingIndexs.length = shift.length;
    }
    return result;
}
