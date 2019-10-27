//
//  AssetBase.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public struct AssetOrder: Comparable {
    //主维度
    public var main: Int64
    
    //二级维度
    public var time: Int64
    
    //三级维度
    public let sequence: Int64
    
    
    public init(main: Int64, time: Int64, sequence: Int64) {
        self.main = main
        self.time = time
        self.sequence = sequence
    }
    public static func < (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedAscending
    }
    public static func > (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedDescending
    }
    public static func <= (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) != .orderedDescending
    }
    public static func >= (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) != .orderedAscending
    }
    public static func == (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedSame
    }
    private static func _compare(lhs: AssetOrder, rhs: AssetOrder) -> ComparisonResult {
        if lhs.main < rhs.main {
            return .orderedAscending
        } else if lhs.main > rhs.main {
            return .orderedDescending
        }
        if lhs.time < rhs.time {
            return .orderedAscending
        } else if lhs.time > rhs.time {
            return .orderedDescending
        }
        if lhs.sequence < rhs.sequence {
            return .orderedAscending
        } else if lhs.sequence > rhs.sequence {
            return .orderedDescending
        }
        return .orderedSame
    }
}
