//
//  a.swift
//  ConnectKitDemo
//
//  Created by CaiLianfeng on 16/8/28.
//  Copyright © 2016年 WangQinghai. All rights reserved.
//

import Foundation


public protocol SequenceProtocol {
    func nextSequenc() -> Self
    func lastSequenc() -> Self
}

extension Int: SequenceProtocol {
    public func nextSequenc() -> Int {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> Int {
        var current = self
        current = current &- 1
        return current
    }
}

extension Int8: SequenceProtocol {
    
    public func nextSequenc() -> Int8 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> Int8 {
        var current = self
        current = current &- 1
        return current
    }
}
extension Int16: SequenceProtocol {
    public func nextSequenc() -> Int16 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> Int16 {
        var current = self
        current = current &- 1
        return current
    }
}

extension Int32: SequenceProtocol {
    public func nextSequenc() -> Int32 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> Int32 {
        var current = self
        current = current &- 1
        return current
    }
}

extension Int64: SequenceProtocol {
    public func nextSequenc() -> Int64 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> Int64 {
        var current = self
        current = current &- 1
        return current
    }
}


extension UInt: SequenceProtocol {
    public func nextSequenc() -> UInt {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> UInt {
        var current = self
        current = current &- 1
        return current
    }
}
extension UInt8: SequenceProtocol {
    public func nextSequenc() -> UInt8 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> UInt8 {
        var current = self
        current = current &- 1
        return current
    }
}
extension UInt16: SequenceProtocol {
    public func nextSequenc() -> UInt16 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> UInt16 {
        var current = self
        current = current &- 1
        return current
    }
}

extension UInt32: SequenceProtocol {
    public func nextSequenc() -> UInt32 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> UInt32 {
        var current = self
        current = current &- 1
        return current
    }
}

extension UInt64: SequenceProtocol {
    public func nextSequenc() -> UInt64 {
        var current = self
        current = current &+ 1
        return current
    }
    public func lastSequenc() -> UInt64 {
        var current = self
        current = current &- 1
        return current
    }
}

open class SequenceGenerator<T: SequenceProtocol> {
    open fileprivate(set) var sequence: T
    
    public init(sequence: T) {
        self.sequence = sequence
    }
    
    open func nextSequence() -> T {
        sequence = sequence.nextSequenc()
        return sequence
    }
}

