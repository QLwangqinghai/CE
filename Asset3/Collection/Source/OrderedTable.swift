//
//  Table.swift
//  Collection
//
//  Created by vector on 2019/11/4.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


public enum TableChange<Element: Any> {
    case remove([(Int, Element)])//(oldIndex, (oldOrder, value))
    case insert([(Int, Element)])//(index, (order, value))
    case update([(Int, Element)])//(index, (order, value))
}

public class ChangeCollection<Value> {
    public typealias Change = CollectionChange<Int, Value>
    
//    fileprivate var array: [Value]
//    public var count: Int {
//        return self.array.count
//    }
//
//    public init() {
//        self.array = []
//    }
//    public func load() -> [Value] {
//        return self.array
//    }
//    public subscript(index: Int) -> Value {
//        return self.array[index]
//    }
//
//    public func removeAll() -> Change {
//        let result: [(Int, Value)] = self.array.enumerated().map { (index, value) -> (Int, Value) in
//            return (index, value)
//        }
//        self.array.removeAll()
//        return .remove(result)
//    }
//
//    public func filter(_ body: (Value) -> Bool) -> Change {
//        var array: [Value] = []
//        var result: [(Int, Value)] = []
//        for (index, value) in self.array.enumerated() {
//            if body(value) {
//                array.append(value)
//            } else {
//                result.append((index, value))
//            }
//        }
//        self.array = array
//        return .remove(result)
//    }
//    public func append(_ items: [Value]) -> Change {
//        let count = self.array.count
//        self.array.append(contentsOf: items)
//        return .insert(items.enumerated().map { (index, value) -> (Int, Value) in
//            return (index + count, value)
//        })
//    }
//    public func append(_ item: Value) -> Change {
//        let count = self.array.count
//        self.array.append(item)
//        return .insert([(count, item)])
//    }
//    public func prepend(_ items: [Value]) -> Change {
//        self.array.insert(contentsOf: items, at: 0)
//        return .insert(items.enumerated().map { (index, value) -> (Int, Value) in
//            return (index, value)
//        })
//    }
//    public func prepend(_ item: Value) -> Change {
//        self.array.insert(item, at: 0)
//        return .insert([(0, item)])
//    }
}

public class Table<Value> {
    public typealias Change = CollectionChange<Int, Value>
    
    fileprivate var array: [Value]
    public var count: Int {
        return self.array.count
    }
    
    public init() {
        self.array = []
    }
    public func load() -> [Value] {
        return self.array
    }
    public subscript(index: Int) -> Value {
        return self.array[index]
    }

    public func removeAll() -> Change {
        let result: [(Int, Value)] = self.array.enumerated().map { (index, value) -> (Int, Value) in
            return (index, value)
        }
        self.array.removeAll()
        return .remove(result)
    }
    
    public func filter(_ body: (Value) -> Bool) -> Change {
        var array: [Value] = []
        var result: [(Int, Value)] = []
        for (index, value) in self.array.enumerated() {
            if body(value) {
                array.append(value)
            } else {
                result.append((index, value))
            }
        }
        self.array = array
        return .remove(result)
    }
    public func append(_ items: [Value]) -> Change {
        let count = self.array.count
        self.array.append(contentsOf: items)
        return .insert(items.enumerated().map { (index, value) -> (Int, Value) in
            return (index + count, value)
        })
    }
    public func append(_ item: Value) -> Change {
        let count = self.array.count
        self.array.append(item)
        return .insert([(count, item)])
    }
    public func prepend(_ items: [Value]) -> Change {
        self.array.insert(contentsOf: items, at: 0)
        return .insert(items.enumerated().map { (index, value) -> (Int, Value) in
            return (index, value)
        })
    }
    public func prepend(_ item: Value) -> Change {
        self.array.insert(item, at: 0)
        return .insert([(0, item)])
    }
}


public struct OrderedTable<Key, Priority, Value> where Key: Hashable, Priority: Comparable, Value: Equatable {
    public struct Element {
        public let key: Key
        public let priority: Priority
        public let value: Value
        public init(key: Key, priority: Priority, value: Value) {
            self.key = key
            self.priority = priority
            self.value = value
        }
    }
    
    public typealias Compare = (_ lhs: Priority, _ rhs: Priority) -> Bool
    public typealias Change = CollectionChange<Int, Element>
    
    public struct Updater {
        public typealias Collection = OrderedTable<Key, Priority, Value>
        
        fileprivate let collection: Collection
        fileprivate var _dictionary: [Key: Element]
        private(set) var compare: Compare

        fileprivate init(collection: Collection, compare: @escaping Compare) {
            self.compare = compare
            self.collection = collection
            self._dictionary = collection.dictionary.mapValues({ (index) -> Element in
                return collection.array[index]
            })
        }
        
        public func uniqueValue(forKey: Key) -> Element? {
            if let result = self._dictionary[forKey] {
                return result
            }
            return self.collection[forKey]
        }
        
        public mutating func removeItem(forKey: Key) {
            self._dictionary.removeValue(forKey: forKey)
        }
        public mutating func replaceItem(_ item: Element, forKey key: Key) {
            self._dictionary[key] = item
        }
        
        public mutating func filter(_ body: (Element) -> Bool) {
            self._dictionary = self._dictionary.filter({ (item) -> Bool in
                return body(item.value)
            })
        }
        public mutating func reset(_ dictionary: [Key: Element]) {
            self._dictionary = dictionary
        }
        public mutating func replace(_ items: [Key: Element]) {
            for item in items {
                self._dictionary[item.key] = item.value
            }
        }
        public mutating func replace<S>(_ items: S) where Element == S.Element, S : Sequence {
            for item in items {
                self._dictionary[item.key] = item
            }
        }
                
        public mutating func removeAll() {
            self._dictionary.removeAll()
        }
        
        public mutating func finish() -> (Collection, [Change]) {
            let list: [Element] = self.collection.array
            let compare: Compare = self.compare
            
            var updated: [(Int, Element)] = []
            var removed: [(Int, Element)] = []
            var remain: [Key: Element] = [:]
            _ = self._dictionary.map { (item) -> Void in
                remain[item.key] = item.value
            }
            
            var array: [Element] = []
            var dictionary: [Key: Int] = [:]
            var changes: [Change] = []
            
            //update & remove
            for (index, arrayItem) in list.enumerated() {
                let key = arrayItem.key
                if let newItem = remain[key] {
                    if arrayItem.priority == newItem.priority {
                        remain.removeValue(forKey: key)
                        array.append(newItem)
                        
                        if (newItem.value != arrayItem.value) {
                            updated.append((index, arrayItem))
                        }
                    } else {
                        removed.append((index, arrayItem))
                    }
                } else {
                    removed.append((index, arrayItem))
                }
            }
            
            if !updated.isEmpty {
                 let change = Change.update(updated)
                 changes.append(change)
             }
            
            if !removed.isEmpty {
                let change = Change.remove(removed)
                changes.append(change)
            }
                    
            var waitInserted: [Element] = remain.map { (item) -> Element in
                return item.value
            }.sorted { (lhs, rhs) -> Bool in
                return compare(lhs.priority, rhs.priority)
            }
            
            if !waitInserted.isEmpty {
                var inserted: [(Int, Element)] = []
                var resultArray: [Element] = []
                
                for item in array {
                    while let insertItem = waitInserted.first {
                        if compare(insertItem.priority, item.priority) {
                            let insertIndex = resultArray.endIndex
                            resultArray.append(insertItem)
                            inserted.append((insertIndex, insertItem))
                            dictionary[insertItem.key] = insertIndex
                            waitInserted.removeFirst()
                        } else {
                            break
                        }
                    }
                    let insertIndex = resultArray.endIndex
                    resultArray.append(item)
                    dictionary[item.key] = insertIndex
                }
                
                while let insertItem = waitInserted.first {
                    let insertIndex = resultArray.endIndex
                    resultArray.append(insertItem)
                    inserted.append((insertIndex, insertItem))
                    dictionary[insertItem.key] = insertIndex
                    waitInserted.removeFirst()
                }
                
                array = resultArray
                if !inserted.isEmpty {
                    let change = Change.insert(inserted)
                    changes.append(change)
                }
            }
            var storage = self.collection
            storage.array = array
            storage.dictionary = dictionary
            return (storage, changes)
        }
    }
    
    public func makeUpdater() -> Updater {
        return Updater(collection: self, compare: self.compare)
    }

    fileprivate var array: [Element]
    fileprivate var dictionary: [Key: Int]

    public var count: Int {
        return self.array.count
    }
    
    public private(set) var order: Order {
        didSet {
            let _order = self.order
            self.compare = { (_ lhs: Priority, _ rhs: Priority) -> Bool in
                switch _order {
                case .ascending:
                    return lhs < rhs
                case .descending:
                    return lhs > rhs
                }
            }
        }
    }
    private(set) var compare: (_ lhs: Priority, _ rhs: Priority) -> Bool
    public init(order: Order = .ascending) {
        self.order = order
        self.array = []
        self.dictionary = [:]
        
        self.compare = { (_ lhs: Priority, _ rhs: Priority) -> Bool in
            switch order {
            case .ascending:
                return lhs < rhs
            case .descending:
                return lhs > rhs
            }
        }
    }
    
    public func load() -> [Value] {
        return self.array.map { (item) -> Value in
            return item.value
        }
    }
    public func object(at index: Int) -> Value {
        return self.array[index].value
    }
    public subscript(index: Int) -> Element {
        return self.array[index]
    }
    public subscript(key: Key) -> Element? {
        guard let index = self.dictionary[key] else {
            return nil
        }
        return self.array[index]
    }
    public func index(of key: Key) -> Int? {
        return self.dictionary[key]
    }
}
