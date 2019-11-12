//
//  Test.swift
//  T
//
//  Created by vector on 2019/11/12.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa


public struct TableKey: Comparable, CustomDebugStringConvertible {
    public struct Generator {
        public static let median: UInt32 = UInt32(Int32.max)
        public static let smallMedian: UInt32 = UInt32(Int32.max) >> 1
        public static let largeMedian: UInt32 = UInt32(Int32.max) + (UInt32(Int32.max) >> 1)
        
        public static func generate() -> TableKey {
            return TableKey(content: [])
        }
        public static func before(_ key: TableKey) -> TableKey {
            var content = key.content
            if !content.isEmpty {
                for i in (0 ..< content.count).reversed() {
                    let v = content[i]
                    if v != UInt32.min {
                        content[i] = v - 1
                        return TableKey(content: content)
                    }
                }
            }
            content.append(self.smallMedian)
            return TableKey(content: content)
        }
        public static func after(_ key: TableKey) -> TableKey {
            var content = key.content
            if !content.isEmpty {
                for i in (0 ..< content.count).reversed() {
                    let v = content[i]
                    if v != UInt32.max {
                        content[i] = v + 1
                        return TableKey(content: content)
                    }
                }
            }
            content.append(self.largeMedian)
            return TableKey(content: content)
        }
        
        public static func between(_ lhs: TableKey, _ rhs: TableKey) -> TableKey {
            let median = Generator.median
            let count = lhs.content.count > rhs.content.count ? lhs.content.count : rhs.content.count
            if (count <= 0) {
                return rhs
            }
            var a: [UInt32] = [UInt32](repeating: median, count: count)
            var b: [UInt32] = [UInt32](repeating: median, count: count)
            var c: [UInt32] = [UInt32](repeating: median, count: count)
            
            for (index, v) in lhs.content.enumerated() {
                a[index] = v
            }
            for (index, v) in rhs.content.enumerated() {
                b[index] = v
            }

            var sum: UInt64 = 0
            for idx in (0 ..< count).reversed() {
                let ci = UInt64(a[idx]) + UInt64(b[idx]) + sum
                c[idx] = UInt32(truncatingIfNeeded: ci)
                sum = ci >> 32
            }
            
            for idx in 0 ..< count {
                let ci = UInt64(c[idx]) + (sum << 32)
                c[idx] = UInt32(truncatingIfNeeded: (ci >> 1))
                sum = ci & 1
            }
            if sum != 0 {
                c.append(Generator.largeMedian)
            }
            
            for idx in 0 ..< count {
                if a[idx] < c[idx] && c[idx] < b[idx] {
                    let removedCount = c.count - idx - 1
                    if removedCount > 0 {
                        c.removeLast(removedCount)
                        break
                    }
                } else if a[idx] > c[idx] && c[idx] > b[idx] {
                    let removedCount = c.count - idx - 1
                    if removedCount > 0 {
                        c.removeLast(removedCount)
                        break
                    }
                }
            }

            return TableKey(content: c)
        }
        
    }
    
    public let content: [UInt32]
    
    private init(content: [UInt32]) {
        self.content = content
    }
    
    public static func compare(lhs: TableKey, rhs: TableKey) -> ComparisonResult {
        let a = lhs.content
        let b = rhs.content
        let median = Generator.median
        if a.count < b.count {
            let count = a.count
            if count > 0 {
                for idx in 0 ..< count {
                    if a[idx] > b[idx] {
                        return .orderedDescending
                    } else if a[idx] < b[idx] {
                        return .orderedAscending
                    }
                }
            }
            for idx in count ..< b.count {
                if median > b[idx] {
                    return .orderedDescending
                } else if median < b[idx] {
                    return .orderedAscending
                }
            }
            return .orderedSame
        } else if a.count > b.count {
            let count = b.count
            if count > 0 {
                for idx in 0 ..< count {
                    if a[idx] > b[idx] {
                        return .orderedDescending
                    } else if a[idx] < b[idx] {
                        return .orderedAscending
                    }
                }
            }
            for idx in count ..< a.count {
                if a[idx] > median {
                    return .orderedDescending
                } else if a[idx] < median {
                    return .orderedAscending
                }
            }
            return .orderedSame
        } else {
            let count = rhs.content.count
            if count > 0 {
                for idx in 0 ..< count {
                    if a[idx] > b[idx] {
                        return .orderedDescending
                    } else if a[idx] < b[idx] {
                        return .orderedAscending
                    }
                }
            }
            return .orderedSame
        }
    }
    
    public static func == (lhs: TableKey, rhs: TableKey) -> Bool {
        self.compare(lhs: lhs, rhs: rhs) == .orderedSame
    }
    
    public static func < (lhs: TableKey, rhs: TableKey) -> Bool {
        self.compare(lhs: lhs, rhs: rhs) == .orderedAscending
    }
    public static func > (lhs: TableKey, rhs: TableKey) -> Bool {
        self.compare(lhs: lhs, rhs: rhs) == .orderedDescending
    }

    public var debugDescription: String {
        return self.content.debugDescription
    }
}


class Test: NSObject {
    static func test() {
        var keys: [TableKey] = []
        keys.append(TableKey.Generator.generate())

        for i in 0 ... 10000 {
            let a = Int(arc4random()) % (keys.count + 1)
            
            if a == 0 {
                keys.insert(TableKey.Generator.before(keys[0]), at: 0)
            } else if a >= keys.count {
                keys.append(TableKey.Generator.after(keys[keys.count - 1]))
            } else {
                keys.insert(TableKey.Generator.between(keys[a-1], keys[a]), at: a)
            }
            print(keys)
            
            for idx in 0 ..< keys.count - 1 {
                if keys[idx] > keys[idx+1] {
                    print("error \(idx) \n")
                }
            }
            
            var length = 0
            for idx in 0 ..< keys.count {
                length += keys[idx].content.count
            }
            let vvv = Double(length) / Double(keys.count)
            print("[count:\(keys.count) length: \(vvv) ]\n")            
        }
    }
}
