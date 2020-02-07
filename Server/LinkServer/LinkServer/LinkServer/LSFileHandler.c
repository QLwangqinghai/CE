//
//  LSFileHandler.c
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSFileHandler.h"
#include "LSTimerQueue.h"


LSSocketSource_s * _Nonnull LSTimerQueueGetReadSource(LSFileHandler_s * _Nonnull handler, uint32_t index);
LSSocketSource_s * _Nonnull LSTimerQueueGetWriteSource(LSFileHandler_s * _Nonnull handler, uint32_t index);


extern void __LSTimerQueueInit(LSTimerQueue_s * _Nonnull queue, LSFileHandler_s * _Nonnull owner, LSTimerQueueGetSource_f _Nonnull getSource);


static inline void __LSConnectInit(LSConnect_s * _Nonnull connect, uint32_t id) {
    connect->readSource.deadline = LSSocketSourceDeadlineForever;
    connect->readSource.index = id;
    connect->writeSource.deadline = LSSocketSourceDeadlineForever;
    connect->writeSource.index = id;
}

void LSFileHandlerInit(LSFileHandler_s * _Nonnull handler, uint32_t id, LSManager_s * _Nonnull manager) {
    assert(handler);
    assert(manager);
    handler->handlerId = id;
    handler->manager = manager;
    
    LSConnect_s * connectTable = CCAlignedAllocate(sizeof(LSConnect_s) * 0x10000, 128);
    for (uint32_t i=0; i<0x10000; i++) {
        __LSConnectInit(connectTable + i, i);
    }
    handler->connectTable = connectTable;
    
    LSTimerQueue_s * readTimerQueue = CCAllocate(sizeof(LSTimerQueue_s));
    __LSTimerQueueInit(readTimerQueue, handler, LSTimerQueueGetReadSource);
    handler->readTimerQueue = readTimerQueue;
    
    LSTimerQueue_s * writeTimerQueue = CCAllocate(sizeof(LSTimerQueue_s));
    __LSTimerQueueInit(writeTimerQueue, handler, LSTimerQueueGetWriteSource);
    handler->writeTimerQueue = writeTimerQueue;
}



//
////不做任何参数校验
//extern void _CEPollRemoveFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file);
//extern void _CEPollRemoveFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file);
//extern void _CEPollAddFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index);
//extern void _CEPollAddFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index);
//extern void _CEPollUpdateFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index);
//extern void _CEPollUpdateFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index);
//
//int _CEPollRemoveFileReadTimersByIndex(CEPoll_s * _Nonnull p, uint32_t index, uint32_t * _Nonnull buffer) {
//    assert(buffer);
//    LSFileTimerMap_s * map = &(p->readTimerMap);
//    uint32_t head = map->table[index];
//    map->table[index] = CEFileIndexInvalid;
//
//    uint32_t iter = head;
//    int count = 0;
//
//    while (iter != CEFileIndexInvalid) {
//        CEFile_t * file = __CEPollGetFileWithIndex(p, iter);
//        assert(1 == file->read.isWaitingTimeout);
//        iter = file->read.next;
//        file->read.prev = CEFileIndexInvalid;
//        file->read.next = CEFileIndexInvalid;
//        file->read.isWaitingTimeout = 0;
//        count += 1;
//        assert(count < 0x10000);
//    }
//    return count;
//}
//
//
//
////不做任何参数校验
//
//void _CEPollRemoveFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file) {
//    assert(1 == file->read.isWaitingTimeout);
//    uint32_t hash = file->read.hashOfTimer;
//    LSFileTimerMap_s * map = &(p->readTimerMap);
//    CEFileSource_s * link = &(file->read);
//    uint32_t prev = link->prev;
//    uint32_t next = link->next;
//    link->prev = CEFileIndexInvalid;
//    link->next = CEFileIndexInvalid;
//
//    if (CEFileIndexInvalid == prev) {
//        uint32_t index = (p->timerTableOffset & 0xE000) + hash;
//        if (index < (p->timerTableOffset & 0x1FFF)) {
//            index -= 0x2000;
//        }
//        for (int i=0; i<3; i++) {
//            uint32_t tmpIndex = index + i * 0x2000;
//            if (map->table[tmpIndex] == fid) {
//                map->table[tmpIndex] = next;
//                return;
//            }
//        }
//        
//        for (int i=1; i<6; i++) {
//            uint32_t tmpIndex = index - i * 0x2000;
//            if (map->table[tmpIndex] == fid) {
//                map->table[tmpIndex] = next;
//                return;
//            }
//        }
//
//        fprintf(stderr, "_CEPollRemoveFileReadTimer prev error, prev=%d\n", prev);
//        fflush(stderr);
//        abort();
//    } else {
//        CEFile_t * prevFile = __CEPollGetFileWithIndex(p, prev);
//        prevFile->read.next = next;
//        if (CEFileIndexInvalid != next) {
//            CEFile_t * nextFile = __CEPollGetFileWithIndex(p, next);
//            nextFile->read.prev = prev;
//        }
//    }
//    file->read.isWaitingTimeout = 0;
//}
//void _CEPollRemoveFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file) {
//    assert(1 == file->write.isWaitingTimeout);
//    uint32_t hash = file->write.hashOfTimer;
//    LSFileTimerMap_s * map = &(p->writeTimerMap);
//    CEFileSource_s * link = &(file->write);
//    uint32_t prev = link->prev;
//    uint32_t next = link->next;
//    link->prev = CEFileIndexInvalid;
//    link->next = CEFileIndexInvalid;
//
//    if (CEFileIndexInvalid == prev) {
//        uint32_t index = (p->timerTableOffset & 0xE000) + hash;
//        if (index < (p->timerTableOffset & 0x1FFF)) {
//            index -= 0x2000;
//        }
//        for (int i=0; i<3; i++) {
//            uint32_t tmpIndex = index + i * 0x2000;
//            if (map->table[tmpIndex] == fid) {
//                map->table[tmpIndex] = next;
//                return;
//            }
//        }
//        
//        for (int i=1; i<6; i++) {
//            uint32_t tmpIndex = index - i * 0x2000;
//            if (map->table[tmpIndex] == fid) {
//                map->table[tmpIndex] = next;
//                return;
//            }
//        }
//
//        fprintf(stderr, "_CEPollRemoveFileWriteTimer prev error, prev=%d\n", prev);
//        fflush(stderr);
//        abort();
//    } else {
//        CEFile_t * prevFile = __CEPollGetFileWithIndex(p, prev);
//        prevFile->write.next = next;
//        if (CEFileIndexInvalid != next) {
//            CEFile_t * nextFile = __CEPollGetFileWithIndex(p, next);
//            nextFile->write.prev = prev;
//        }
//    }
//    file->write.isWaitingTimeout = 0;
//}
//void _CEPollAddFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index) {
//    assert(0 == file->read.isWaitingTimeout);
//    LSFileTimerMap_s * map = &(p->readTimerMap);
//    CEFileSource_s * link = &(file->read);
//    link->prev = CEFileIndexInvalid;
//    link->next = CEFileIndexInvalid;
//    uint32_t prev = map->table[index];
//    map->table[index] = fid;
//    
//    if (CEFileIndexInvalid != prev) {
//        CEFile_t * nextFile = __CEPollGetFileWithIndex(p, prev);
//        nextFile->read.prev = fid;
//    }
//    file->read.isWaitingTimeout = 1;
//}
//void _CEPollAddFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index) {
//    assert(0 == file->write.isWaitingTimeout);
//    LSFileTimerMap_s * map = &(p->writeTimerMap);
//    CEFileSource_s * link = &(file->write);
//    link->prev = CEFileIndexInvalid;
//    link->next = CEFileIndexInvalid;
//    uint32_t prev = map->table[index];
//    map->table[index] = fid;
//    if (CEFileIndexInvalid != prev) {
//        CEFile_t * nextFile = __CEPollGetFileWithIndex(p, prev);
//        nextFile->write.prev = fid;
//    }
//    file->write.isWaitingTimeout = 1;
//}
//void _CEPollUpdateFileReadTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index) {
//    if (0 == file->read.isWaitingTimeout) {
//        _CEPollRemoveFileReadTimer(p, fid, file);
//    }
//    _CEPollAddFileReadTimer(p, fid, file, index);
//}
//void _CEPollUpdateFileWriteTimer(CEPoll_s * _Nonnull p, uint32_t fid, CEFile_t * _Nonnull file, uint32_t index) {
//    if (0 == file->write.isWaitingTimeout) {
//        _CEPollRemoveFileWriteTimer(p, fid, file);
//    }
//    _CEPollAddFileWriteTimer(p, fid, file, index);
//}


LSSocketSource_s * _Nonnull LSTimerQueueGetReadSource(LSFileHandler_s * _Nonnull handler, uint32_t index) {
    return &(handler->connectTable[index].readSource);
}
LSSocketSource_s * _Nonnull LSTimerQueueGetWriteSource(LSFileHandler_s * _Nonnull handler, uint32_t index) {
    return &(handler->connectTable[index].writeSource);
}


void _LSFileHandlerFileEventCallback(CCPtr _Nullable context, CEFileFiredInfo * _Nonnull infos, uint32_t count) {
    
}
