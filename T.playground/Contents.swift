import Cocoa

//
//var string = ""
//var codes = [Int32]()
//
//var items = [(Int32, String)]()
//for a in 0 ..< 255 {
//    let char: Int32 = Int32(a)
//    if isprint(char) != 0 {
//        let str = String(format: "%C", a, char)
//        string += str
//        codes.append(char)
//        items.append((char, str))
//    }
//}
//
//print(codes.count)
//
//
//print(string)
//print(codes)
//
//for (index, item) in items.enumerated() {
//    print("\(index): \(item)")
//}
//
//for i in (0 ..< 5).reversed() {
//    print(i)
//}
//
//print(items)
public struct TableKey: Comparable, CustomDebugStringConvertible {
    public struct Generator {
        public static let median: UInt32 = UInt32(Int32.max)
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
            content.append(self.median)
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
            content.append(self.median)
            return TableKey(content: content)
        }
        
        public static func between(_ lhs: TableKey, _ rhs: TableKey) -> TableKey {
            let a: [UInt32]
            let b: [UInt32]
            var c: [UInt32] = []
            if lhs.content.count < rhs.content.count {
                a = lhs.content
                b = rhs.content
            } else if lhs.content.count > rhs.content.count {
                a = rhs.content
                b = lhs.content
            } else {
                a = lhs.content
                b = rhs.content
            }
            let count = a.count
            var swap: Bool? = nil
            if count > 0 {
                for idx in 0 ..< count {
                    let ai = UInt64(a[idx])
                    let bi = UInt64(b[idx])
                    if ai < bi {
                        result = false
                    } else if ai < bi {
                        result = .orderedDescending
                    } else {
                        
                    }
                    let ci = ai + bi / 2
                    c.append(UInt32(ci))
                    if ci > ai && ci < bi {
                        return TableKey(content: c)
                    } else if ci < ai && ci > bi {
                        return TableKey(content: c)
                    }
                }
            }
            
            if b.count > a.count {
                for idx in count ..< b.count {
                    let ai: UInt64 = 0
                    let bi = UInt64(b[idx])
                    let ci = ai + bi / 2
                    c.append(UInt32(ci))
                    if ci > ai && ci < bi {
                        return TableKey(content: c)
                    } else if ci < ai && ci > bi {
                        return TableKey(content: c)
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
        if a.count < b.count {
            let count = a.count
            if count > 0 {
                for idx in 0 ..< count {
                    if lhs.content[idx] > rhs.content[idx] {
                        return .orderedDescending
                    } else if lhs.content[idx] < rhs.content[idx] {
                        return .orderedAscending
                    }
                }
            }
            return .orderedAscending
        } else if a.count > b.count {
            let count = b.count
            if count > 0 {
                for idx in 0 ..< count {
                    if lhs.content[idx] > rhs.content[idx] {
                        return .orderedDescending
                    } else if lhs.content[idx] < rhs.content[idx] {
                        return .orderedAscending
                    }
                }
            }
            return .orderedDescending
        } else {
            let count = rhs.content.count
            if count > 0 {
                for idx in 0 ..< count {
                    if lhs.content[idx] > rhs.content[idx] {
                        return .orderedDescending
                    } else if lhs.content[idx] < rhs.content[idx] {
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
    sleep(3)
}


