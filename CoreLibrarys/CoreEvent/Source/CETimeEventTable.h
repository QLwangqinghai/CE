//
//  CETimeEventTable.h
//  CoreEvent
//
//  Created by vector on 2018/12/14.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimeEventTable_h
#define CETimeEventTable_h

#include <CoreEvent/CEBaseTypeInternal.h>
#include <CoreEvent/CEMemory.h>


static inline uint32_t CEBitTableUtilGetUnusedIndex64(uint64_t n) {
    if (UINT64_MAX == n) {
        return UINT32_MAX;
    }
    
    uint32_t i = 0;
    uint32_t j = 63;
    uint32_t n1 = (uint32_t)(n >> 32);
    if (n1 == UINT32_MAX) {
        i += 32;
        n1 = (uint32_t)(n & UINT32_MAX);
    } else {
        j -= 32;
    }
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}
static inline uint32_t CEBitTableUtilGetUnusedIndex32(uint32_t n) {
    if (UINT32_MAX == n) {
        return UINT32_MAX;
    }
    uint32_t i = 0;
    uint32_t j = 31;
    uint32_t n1 = n;
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}
static inline uint32_t CEBitTableUtilGetUnusedIndex16(uint16_t n) {
    if (UINT16_MAX == n) {
        return UINT32_MAX;
    }
    uint32_t i = 0;
    uint32_t j = 15;
    uint16_t n2 = n;
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}


static inline _Bool CEBitTable16AllocateKey(CEBitTable16_s * _Nonnull bitTable, uint32_t * _Nonnull keyPtr) {
    assert(keyPtr);
    uint64_t mask = 0x8000000000000000ull;
    
    uint32_t id = 0;
    uint16_t n0 = bitTable->n0;
    
    uint32_t offset0 = CEBitTableUtilGetUnusedIndex16(n0);
    if (offset0 > 15) {
        return false;
    }
    
    uint32_t index1 = offset0;
    uint64_t n1 = bitTable->n1[index1];
    uint32_t offset1 = CEBitTableUtilGetUnusedIndex64(n1);
    if (offset1 > 63) {
        abort();
    }
    
    uint32_t index2 = offset1 + (index1 << 6);
    uint64_t n2 = bitTable->n2[index2];
    uint32_t offset2 = CEBitTableUtilGetUnusedIndex64(n2);
    if (offset2 > 63) {
        abort();
    }
    n2 = n2 | (mask >> offset2);
    bitTable->n2[index2] = n2;
    if (UINT64_MAX == n2) {
        n1 = n1 | (mask >> offset1);
        bitTable->n1[index1] = n1;
        
        if (UINT64_MAX == n1) {
            uint16_t m16 = 0x8000;
            n0 = n0 | (m16 >> offset0);
            bitTable->n0 = n0;
        }
    }
    id = (uint32_t)((index2 << 6) + offset2);
    
    bitTable->usedCount += 1;
    
    *keyPtr = id;
    return true;
}

static inline void CEBitTable16DeallocateKey(CEBitTable16_s * _Nonnull bitTable, uint32_t key) {
    if (key >= UINT16_MAX) {
        return;
    }
    uint64_t mask = 0x8000000000000000ull;
    uint16_t m16 = 0x8000;
    
    uint16_t n0 = bitTable->n0;
    uint32_t offset0 = key >> 12;
    
    uint32_t index1 = offset0;
    uint32_t offset1 = (key >> 6) & 0x3F;
    uint64_t n1 = bitTable->n1[index1];
    
    uint32_t index2 = offset1 + (index1 << 6);
    uint32_t offset2 = key & 0x3F;
    uint64_t n2 = bitTable->n2[index2];
    
    assert( n2 & (mask >> offset2));
    n2 = n2 & (~(mask >> offset2));
    bitTable->n2[index2] = n2;
    
    n1 = n1 & (~(mask >> offset1));
    bitTable->n1[index1] = n1;
    
    n0 = n0 & (~(m16 >> offset0));
    bitTable->n0 = n0;
    bitTable->usedCount -= 1;
}

static inline CETimeEvent_s * _Nullable CETimeEventTableGetItem(CETimeEventTable_s * _Nonnull table, uint32_t key) {
    uint32_t pageIndex = key >> 10;
    if (pageIndex >= 64) {
        return NULL;
    }
    CETimeEventTablePage_s * page = table->pages[pageIndex];
    if (NULL == page) {
        return NULL;
    }
    uint32_t itemIndex = key & 0x3FF;
    
    return page->items[itemIndex];
}

static inline CETimeEvent_s * _Nullable CETimeEventTableRemoveItem(CETimeEventTable_s * _Nonnull table, uint32_t key) {
    uint32_t pageIndex = key >> 10;
    if (pageIndex >= 64) {
        return NULL;
    }
    CETimeEventTablePage_s * page = table->pages[pageIndex];
    if (NULL == page) {
        return NULL;
    }
    uint32_t itemIndex = key & 0x3FF;
    
    CETimeEvent_s * item = page->items[itemIndex];
    page->items[itemIndex] = NULL;
    return item;
}
static inline _Bool CETimeEventTableAddItem(CETimeEventTable_s * _Nonnull table, uint32_t key, CETimeEvent_s * _Nonnull item) {
    assert(item);
    uint32_t pageIndex = key >> 10;
    if (pageIndex >= 64) {
        return false;
    }
    CETimeEventTablePage_s * page = table->pages[pageIndex];
    if (NULL == page) {
        page = CEAllocateClear(sizeof(CETimeEventTablePage_s));
        table->pages[pageIndex] = page;
    }
    uint32_t itemIndex = key & 0x3FF;
    
    CETimeEvent_s * old = page->items[itemIndex];
    if (NULL != old) {
        return false;
    }
    page->items[itemIndex] = item;
    return true;
}



#endif /* CETimeEventTable_h */
