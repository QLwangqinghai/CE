//
//  CCInit.c
//  Basic
//
//  Created by vector on 2020/2/2.
//

#include "CCBase.h"
#include <assert.h>


#pragma mark - Init

__attribute__((constructor(101)))
static void __CCInit(void) {
    assert(UINT_FAST32_MAX == UINT32_MAX);
    assert(UINT_FAST64_MAX == UINT64_MAX);
    assert(sizeof(uintptr_t) == sizeof(UInteger));
}
