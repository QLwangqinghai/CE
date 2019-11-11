import Cocoa


var string = ""
var codes = [Int32]()

var items = [(Int32, String)]()
for a in 0 ..< 255 {
    let char: Int32 = Int32(a)
    if isprint(char) != 0 {
        let str = String(format: "%C", a, char)
        string += str
        codes.append(char)
        items.append((char, str))
    }
}

print(codes.count)


print(string)
print(codes)

for (index, item) in items.enumerated() {
    print("\(index): \(item)")
}

for i in (0 ..< 5).reversed() {
    print(i)
}

print(items)
public struct TableKey: Comparable {
    public struct Generator {
        public static func generate() -> TableKey {
            return TableKey(content: [])
        }
        public static func before(_ key: TableKey) -> TableKey {
            var content = key.content
            if !content.isEmpty {
                for i in (0 ..< content.count).reversed() {
                    let v = content[i]
                    if v != Int.min {
                        content[i] = v - 1
                        return TableKey(content: content)
                    }
                }
            }
            content.append(-1)
            return TableKey(content: content)
        }
        public static func after(_ key: TableKey) -> TableKey {
            var content = key.content
            if !content.isEmpty {
                for i in (0 ..< content.count).reversed() {
                    let v = content[i]
                    if v != Int.max {
                        content[i] = v + 1
                        return TableKey(content: content)
                    }
                }
            }
            content.append(1)
            return TableKey(content: content)
        }
        
        public static func between(_ lhs: TableKey, _ rhs: TableKey) -> TableKey {
            if lhs.content.count < rhs.content.count {
                let count = lhs.content.count
                if count > 0 {
                    for idx in 0 ..< count {
                        if lhs.content[idx] > rhs.content[idx] {
                            return .orderedDescending
                        } else if lhs.content[idx] < rhs.content[idx] {
                            return .orderedAscending
                        }
                    }
                }
                
                for idx in count ..< lhs.content.count {
                    if lhs.content[idx] > rhs.content[idx] {
                        return .orderedDescending
                    } else if lhs.content[idx] < rhs.content[idx] {
                        return .orderedAscending
                    }
                }
                return .orderedSame
            } else if lhs.content.count > rhs.content.count {
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
                
                for idx in count ..< rhs.content.count {
                    if lhs.content[idx] > 0 {
                        return .orderedDescending
                    } else if lhs.content[idx] < 0 {
                        return .orderedAscending
                    }
                }
                return .orderedSame
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

            
            
            
            var content: [Int] = []
            let compareResult = TableKey.compare(lhs: lhs, rhs: rhs)
            switch compareResult {
            case .orderedSame:
                return lhs
            case .orderedAscending:
                 
            case .orderedDescending:
      
            }
            let count = lhs.content.count < rhs.content.count ? lhs.content.count : rhs.content.count
            
            return TableKey(content: [])

        }
        
    }
    
    public let content: [Int]
    
    private init(content: [Int]) {
        self.content = content
    }
    
    public static func compare(lhs: TableKey, rhs: TableKey) -> ComparisonResult {
        if lhs.content.count < rhs.content.count {
            let count = lhs.content.count
            if count > 0 {
                for idx in 0 ..< count {
                    if lhs.content[idx] > rhs.content[idx] {
                        return .orderedDescending
                    } else if lhs.content[idx] < rhs.content[idx] {
                        return .orderedAscending
                    }
                }
            }
            
            for idx in count ..< lhs.content.count {
                if lhs.content[idx] > rhs.content[idx] {
                    return .orderedDescending
                } else if lhs.content[idx] < rhs.content[idx] {
                    return .orderedAscending
                }
            }
            return .orderedSame
        } else if lhs.content.count > rhs.content.count {
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
            
            for idx in count ..< rhs.content.count {
                if lhs.content[idx] > 0 {
                    return .orderedDescending
                } else if lhs.content[idx] < 0 {
                    return .orderedAscending
                }
            }
            return .orderedSame
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
}
