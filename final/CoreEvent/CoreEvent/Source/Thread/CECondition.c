//
//  CECondition.c
//  CoreEvent
//
//  Created by vector on 2019/6/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CECondition.h"


void CEConditionInit(CEConditionPtr _Nonnull lock) {
    pthread_mutex_init(&(lock->mutex), NULL);
    pthread_cond_init(&(lock->cond), NULL);
}

void CEConditionDestroy(CEConditionPtr _Nonnull lock) {
    pthread_mutex_destroy(&(lock->mutex));
    pthread_cond_destroy(&(lock->cond));
}

void CEConditionWait(CEConditionPtr _Nonnull lock) {
    
    
}
void CEConditionSignal(CEConditionPtr _Nonnull lock) {
    
    
    
}



