//
//  main.c
//  TFork
//
//  Created by vector on 2018/12/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <dispatch/dispatch.h>


int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
    printf("%ld", sizeof(timer));
//    dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, intervalInSeconds * NSEC_PER_SEC, <#leewayInSeconds#> * NSEC_PER_SEC);
//    dispatch_source_set_event_handler(timer, ^{
//        <#code to be executed when timer fires#>
//    });
//    dispatch_resume(timer);
    
    fork();
    
//    pthread_atfork(<#void (* _Nullable)(void)#>, <#void (* _Nullable)(void)#>, <#void (* _Nullable)(void)#>)

    return 0;
}
