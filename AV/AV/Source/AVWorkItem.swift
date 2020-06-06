//
//  AVWorkItem.swift
//  AV
//
//  Created by vector on 2019/7/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation


open class AVWorkItem {
    private static var _nextSequence: UInt64 = 0
    private static let _lock: NSLock = NSLock()
    public static var nextSequence: UInt64 {
        _lock.lock()
        _nextSequence += 1
        let a: UInt64 = _nextSequence
        _lock.unlock()
        return a
    }

    //数越小， 优先级越高
    fileprivate var priority: UInt64
    public let identifier: UInt64
    private var completion: ((AVWorkItem) -> Void)?
    
    public init() {
        let identifier = AVWorkItem.nextSequence
        self.identifier = identifier
        self.priority = identifier
    }
    
    open func start(completion: (AVWorkItem) -> Void) {
        
        let a: Array<Int> = []
        
        
        
    }
    open func cancel() {
        
    }
    
    
    open func finish() {
        guard let completion = self.completion else {
            return
        }
        completion(self)
        self.completion = nil
    }
}

public class AVWorkQueue {
    private var items: [AVWorkItem] = []
    
    
}


