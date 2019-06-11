//
//  Swift.swift
//  Demo2
//
//  Created by vector on 2019/5/30.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

import Foundation
import Dispatch

#if CYGWIN
private typealias _MutexPointer = UnsafeMutablePointer<pthread_mutex_t?>
private typealias _ConditionVariablePointer = UnsafeMutablePointer<pthread_cond_t?>
#else
private typealias _MutexPointer = UnsafeMutablePointer<pthread_mutex_t>
private typealias _ConditionVariablePointer = UnsafeMutablePointer<pthread_cond_t>
#endif

open class NSCondition: NSObject {
    internal var mutex = _MutexPointer.allocate(capacity: 1)
    internal var cond = _ConditionVariablePointer.allocate(capacity: 1)
    
    public override init() {
        pthread_mutex_init(mutex, nil)
        pthread_cond_init(cond, nil)
    }
    
    deinit {
        pthread_mutex_destroy(mutex)
        pthread_cond_destroy(cond)
        mutex.deinitialize(count: 1)
        cond.deinitialize(count: 1)
        mutex.deallocate()
        cond.deallocate()
    }
    
    open func lock() {
        pthread_mutex_lock(mutex)
    }
    
    open func unlock() {
        pthread_mutex_unlock(mutex)
    }
    
    open func wait() {
        pthread_cond_wait(cond, mutex)
    }

    open func signal() {
        pthread_cond_signal(cond)
    }
    
    open func broadcast() {
        pthread_cond_broadcast(cond)
    }
    
    open var name: String?
}



open class NSConditionLock : NSObject {
    internal var _cond = NSCondition()
    internal var _value: Int
    internal var _thread: pthread_t?
    
    public convenience override init() {
        self.init(condition: 0)
    }
    
    public init(condition: Int) {
        _value = condition
    }
    
    open var condition: Int {
        return _value
    }
    
    open func unlock(withCondition condition: Int) {
        _cond.lock()
        _thread = nil
        _value = condition
        _cond.broadcast()
        _cond.unlock()
    }
    
    open func lock(whenCondition condition: Int) {
        _cond.lock()
        while _thread != nil || _value != condition {
            _cond.wait()
        }
        _thread = pthread_self()
        _cond.unlock()
    }
    
    open var name: String?
}
