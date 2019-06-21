//
//  CETest.swift
//  TestCE
//
//  Created by vector on 2019/6/16.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

import Foundation
import CoreEvent

public enum CEQueuePriority: UInt16 {
    case high = 192
    case `default` = 128
    case low = 64
}


public class CEQueue {
    public static let perform: CEFunction_f = { (obj, param, result) in
        autoreleasepool(invoking: { () -> Void in
            let item: DispatchWorkItem = Unmanaged<DispatchWorkItem>.fromOpaque(obj).takeUnretainedValue()
            item.perform()
        })
    };
    
    public static let finish: CETaskFinish_f = { (obj) in
        autoreleasepool(invoking: { () -> Void in
            _ = Unmanaged<DispatchWorkItem>.fromOpaque(obj).autorelease()
        })
    }
    

    
    public let queue: CEQueueRef
    public init(_ queue: CEQueueRef) {
        self.queue = queue
    }
    
    public func async(block: @escaping @convention(block) () -> Void) {
        self.async(DispatchWorkItem(block: block))
    }
    public func sync(block: @escaping @convention(block) () -> Void) {
        self.sync(DispatchWorkItem(block: block))
    }
    public func async(_ item: DispatchWorkItem) {
        CEQueueAsync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), CEQueue.finish, CEQueue.perform, nil, CETaskTagNext())
    }
    public func sync(_ item: DispatchWorkItem) {
        CEQueueSync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), CEQueue.finish, CEQueue.perform, nil, nil, CETaskTagNext())
    }
}

final public class CESerialQueue : CEQueue {
    public let label: String
    public let priority: CEQueuePriority
    public static let main: CESerialQueue = CESerialQueue(queue: CEQueueSharedMainQueue(), label: "main", priority: .high)

    
    private init(queue: CEQueueRef, label: String, priority: CEQueuePriority = .default) {
        self.label = label
        self.priority = priority
        super.init(queue)
    }
    
    public convenience init(label: String, priority: CEQueuePriority = .default) {
        let q = label.withCString { (l) -> CEQueueRef in
            return CESerialQueueCreate(l, priority.rawValue)
        }
        self.init(queue: q, label: label, priority: priority)
    }
}

final public class CEConcurrentQueue : CEQueue {
    public let label: String
    public let priority: CEQueuePriority
    
    private init(queue: CEQueueRef, label: String, concurrencyCount: UInt16, priority: CEQueuePriority = .default) {
        self.label = label
        self.priority = priority
        super.init(queue)
    }
    
    public convenience init(label: String, concurrencyCount: UInt16, priority: CEQueuePriority = .default) {
        let q = label.withCString { (l) -> CEQueueRef in
            return CEConcurrentQueueCreate(l, priority.rawValue, 64)
        }
        self.init(queue: q, label: label, concurrencyCount:concurrencyCount, priority: priority)
    }
    
    public func barrierAsync(_ item: DispatchWorkItem) {
        CEConcurrentQueueBarrierAsync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), CEQueue.finish, CEQueue.perform, nil, CETaskTagNext())
    }
    public func barrierSync(_ item: DispatchWorkItem) {
        CEConcurrentQueueBarrierSync(self.queue, Unmanaged<DispatchWorkItem>.passRetained(item).toOpaque(), CEQueue.finish, CEQueue.perform, nil, nil, CETaskTagNext())
    }
}

public class P {
    let t: String
    init(_ t: String) {
        self.t = t
    }
    deinit {
        print("deinit \(self.t)")
    }
}



open class S : NSObject {
    
    public static let queue: CESerialQueue = CESerialQueue.init(label: "my0")
    public static let cQueue: CEConcurrentQueue = CEConcurrentQueue.init(label: "my2", concurrencyCount: 64)

    public class func t0() {
        let p1 = P("0")
        CESerialQueue.main.async(DispatchWorkItem(block: {
            print("0=\(p1)")
        }))
    }
    public class func t1() {
        let p2 = P("1")

        CESerialQueue.main.async(DispatchWorkItem(block: {
            print("1=\(p2)")
        }))
    }
    
    
    public class func t2() {
        let p = P("2")
        S.queue.async(DispatchWorkItem(block: {
            print("2=\(p)")
        }))
    }
    public class func t3() {
        let p = P("3")
        S.queue.async(DispatchWorkItem(block: {
            print("3=\(p)")
        }))
    }
    public class func t4() {
        for i in 1...640 {
            let p = P("\(i)")


            
            if i % 128 == 0 {
                S.cQueue.barrierAsync(DispatchWorkItem(block: {
                    sleep(1)
                    print("item finish: \(p)")
                }))
            } else {
                S.cQueue.async(DispatchWorkItem(block: {
                    sleep(1)
                    print("item finish: \(p)")
                }))
            }
        }
    }
    
    @objc public class func main() {
        
//        t0()
//        t1()
//        t2()
//        t3()
        t4()
        



        CEQueueMain()

    }
    
    
}
