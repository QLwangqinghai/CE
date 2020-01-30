//
//  CEPollFile.c
//  CoreEvent
//
//  Created by vector on 2020/1/29.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CEPollInternal.h"


int _CEPollRemoveFileReadTimersByIndex(CEPoll_s * _Nonnull p, uint16_t index, uint16_t * _Nonnull buffer) {
    assert(buffer);
    CEFileTimerMap_s * map = &(p->readTimerMap);
    uint16_t head = map->table[index];
    map->table[index] = CEFileIndexInvalid;

    uint16_t iter = head;
    int count = 0;

    while (iter != CEFileIndexInvalid) {
        CEFile_s * file = CEPollGetFile(p, iter);
        assert(1 == file->checkReadTimeout);
        iter = file->readTimer.next;
        file->readTimer.prev = CEFileIndexInvalid;
        file->readTimer.next = CEFileIndexInvalid;
        file->checkReadTimeout = 0;
        count += 1;
        assert(count < 0x10000);
    }
    return count;
}



//不做任何参数校验

void _CEPollRemoveFileReadTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file) {
    assert(1 == file->checkReadTimeout);
    uint16_t hash = file->hashOfReadTimer;
    CEFileTimerMap_s * map = &(p->readTimerMap);
    CEFileLink_s * link = &(file->readTimer);
    uint16_t prev = link->prev;
    uint16_t next = link->next;
    link->prev = CEFileIndexInvalid;
    link->next = CEFileIndexInvalid;

    if (CEFileIndexInvalid == prev) {
        uint16_t index = (p->timerTableOffset & 0xE000) + hash;
        if (index < (p->timerTableOffset & 0x1FFF)) {
            index -= 0x2000;
        }
        for (int i=0; i<3; i++) {
            uint16_t tmpIndex = index + i * 0x2000;
            if (map->table[tmpIndex] == fid) {
                map->table[tmpIndex] = next;
                return;
            }
        }
        
        for (int i=1; i<6; i++) {
            uint16_t tmpIndex = index - i * 0x2000;
            if (map->table[tmpIndex] == fid) {
                map->table[tmpIndex] = next;
                return;
            }
        }

        fprintf(stderr, "_CEPollRemoveFileReadTimer prev error, prev=%d\n", prev);
        fflush(stderr);
        abort();
    } else {
        CEFile_s * prevFile = CEPollGetFile(p, prev);
        prevFile->readTimer.next = next;
        if (CEFileIndexInvalid != next) {
            CEFile_s * nextFile = CEPollGetFile(p, next);
            nextFile->readTimer.prev = prev;
        }
    }
    file->checkReadTimeout = 0;
}
void _CEPollRemoveFileWriteTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file) {
    assert(1 == file->checkWriteTimeout);
    uint16_t hash = file->hashOfWriteTimer;
    CEFileTimerMap_s * map = &(p->writeTimerMap);
    CEFileLink_s * link = &(file->writeTimer);
    uint16_t prev = link->prev;
    uint16_t next = link->next;
    link->prev = CEFileIndexInvalid;
    link->next = CEFileIndexInvalid;

    if (CEFileIndexInvalid == prev) {
        uint16_t index = (p->timerTableOffset & 0xE000) + hash;
        if (index < (p->timerTableOffset & 0x1FFF)) {
            index -= 0x2000;
        }
        for (int i=0; i<3; i++) {
            uint16_t tmpIndex = index + i * 0x2000;
            if (map->table[tmpIndex] == fid) {
                map->table[tmpIndex] = next;
                return;
            }
        }
        
        for (int i=1; i<6; i++) {
            uint16_t tmpIndex = index - i * 0x2000;
            if (map->table[tmpIndex] == fid) {
                map->table[tmpIndex] = next;
                return;
            }
        }

        fprintf(stderr, "_CEPollRemoveFileWriteTimer prev error, prev=%d\n", prev);
        fflush(stderr);
        abort();
    } else {
        CEFile_s * prevFile = CEPollGetFile(p, prev);
        prevFile->writeTimer.next = next;
        if (CEFileIndexInvalid != next) {
            CEFile_s * nextFile = CEPollGetFile(p, next);
            nextFile->writeTimer.prev = prev;
        }
    }
    file->checkWriteTimeout = 0;
}
void _CEPollAddFileReadTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file, uint16_t index) {
    assert(0 == file->checkReadTimeout);
    CEFileTimerMap_s * map = &(p->readTimerMap);
    CEFileLink_s * link = &(file->readTimer);
    link->prev = CEFileIndexInvalid;
    link->next = CEFileIndexInvalid;
    uint16_t prev = map->table[index];
    map->table[index] = fid;
    
    if (CEFileIndexInvalid != prev) {
        CEFile_s * nextFile = CEPollGetFile(p, prev);
        nextFile->readTimer.prev = fid;
    }
    file->checkReadTimeout = 1;
}
void _CEPollAddFileWriteTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file, uint16_t index) {
    assert(0 == file->checkWriteTimeout);
    CEFileTimerMap_s * map = &(p->writeTimerMap);
    CEFileLink_s * link = &(file->writeTimer);
    link->prev = CEFileIndexInvalid;
    link->next = CEFileIndexInvalid;
    uint16_t prev = map->table[index];
    map->table[index] = fid;
    if (CEFileIndexInvalid != prev) {
        CEFile_s * nextFile = CEPollGetFile(p, prev);
        nextFile->writeTimer.prev = fid;
    }
    file->checkReadTimeout = 1;
}
void _CEPollUpdateFileReadTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file, uint16_t index) {
    if (0 == file->checkReadTimeout) {
        _CEPollRemoveFileReadTimer(p, fid, file);
    }
    _CEPollAddFileReadTimer(p, fid, file, index);
}
void _CEPollUpdateFileWriteTimer(CEPoll_s * _Nonnull p, uint16_t fid, CEFile_s * _Nonnull file, uint16_t index) {
    if (0 == file->checkWriteTimeout) {
        _CEPollRemoveFileWriteTimer(p, fid, file);
    }
    _CEPollAddFileWriteTimer(p, fid, file, index);
}


