//
//  CETest.swift
//  TestCE
//
//  Created by vector on 2019/6/16.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

import Foundation
import CoreEvent


public class CEQueue {
    public let queue: CEQueueRef
    public init(_ queue: CEQueueRef) {
        self.queue = queue
    }
    public func async(_ item: DispatchWorkItem) {
        CEQueueAsync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), { (obj) in
            _ = Unmanaged<DispatchWorkItem>.fromOpaque(obj).autorelease()
        }, { (obj, param, result) in
            let item: DispatchWorkItem = Unmanaged<DispatchWorkItem>.fromOpaque(obj).takeRetainedValue()
            item.perform()
        }, nil)
    }
    public func sync(_ item: DispatchWorkItem) {
        CEQueueSync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), { (obj) in
            _ = Unmanaged<DispatchWorkItem>.fromOpaque(obj).autorelease()
        }, { (obj, param, result) in
            let item: DispatchWorkItem = Unmanaged<DispatchWorkItem>.fromOpaque(obj).takeRetainedValue()
            item.perform()
        }, nil, nil)
    }
}

public class CESerialQueue : CEQueue {
    public static let main: CESerialQueue = CESerialQueue(CEQueueSharedMainQueue())


}

public class CEConcurrentQueue : CEQueue {
    
    
}




open class S : NSObject {
    
    
    
    
    @objc public class func main() {
        
        CESerialQueue.main.async(DispatchWorkItem(block: {
            print("123")
        }))
        
        
        CEQueueMain()

    }
    
    
}
