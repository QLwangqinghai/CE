//
//  main.c
//  TestC11Atomic
//
//  Created by vector on 2018/12/10.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>

#include <stdatomic.h>
#if __has_include(<netdb.h>)
#include <netdb.h> // for Host.swift
#endif

#if __has_include(<ifaddrs.h>)
#include <ifaddrs.h> // for Host.swift
#endif




static _Atomic(uint_fast64_t) atomicValue64 = ATOMIC_VAR_INIT(0);


int main(int argc, const char * argv[]) {
    // insert code here...
    
    uint64_t oldValue = 3;
    uint64_t newValue = 3;

    _Bool result = atomic_compare_exchange_weak(&(atomicValue64), &oldValue, newValue);

    printf("%d, oldValue:%llu, newValue:%llu\n", result ? 1 : 0, oldValue, newValue);
    

    _Bool result1 = atomic_compare_exchange_weak(&(atomicValue64), &oldValue, newValue);
    
    printf("%d, oldValue:%llu, newValue:%llu\n", result ? 1 : 0, oldValue, newValue);

    
    return 0;
}
