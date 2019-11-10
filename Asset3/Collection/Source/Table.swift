//
//  PriorityDictionary.swift
//  Asset
//
//  Created by vector on 2019/11/4.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum TableOrder {
    case ascending
    case descending
}

public enum TableChange<Element: Any> {
    case remove([(Int, Element)])
    case insert([(Int, Element)])
    case update([(Int, Element)])
}

public struct Table<Key, Priority, Value> where Key: Hashable, Priority: Comparable, Value: Equatable {
    public struct Row {
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
    public typealias Change = TableChange<Row>
    
    public struct Updater {
        public typealias Collection = Table<Key, Priority, Value>
        
        fileprivate let collection: Collection
        fileprivate var _dictionary: [Key: Row]
        private(set) var compare: Compare

        fileprivate init(collection: Collection, compare: @escaping Compare) {
            self.compare = compare
            self.collection = collection
            self._dictionary = collection.dictionary.mapValues({ (index) -> Row in
                return collection.array[index]
            })
        }
        
        public func uniqueValue(forKey: Key) -> Row? {
            if let result = self._dictionary[forKey] {
                return result
            }
            return self.collection[forKey]
        }
        
        public mutating func removeItem(forKey: Key) {
            self._dictionary.removeValue(forKey: forKey)
        }
        public mutating func replaceItem(_ item: Row, forKey key: Key) {
            self._dictionary[key] = item
        }
        
        public mutating func filter(_ body: (Row) -> Bool) {
            self._dictionary = self._dictionary.filter({ (item) -> Bool in
                return body(item.value)
            })
        }
        public mutating func reset(_ dictionary: [Key: Row]) {
            self._dictionary = dictionary
        }
        public mutating func replace(_ items: [Key: Row]) {
            for item in items {
                self._dictionary[item.key] = item.value
            }
        }
        public mutating func replace<S>(_ items: S) where Row == S.Element, S : Sequence {
            for item in items {
                self._dictionary[item.key] = item
            }
        }
                
        public mutating func removeAll() {
            self._dictionary.removeAll()
        }
        
        public mutating func finish() -> (Collection, [Change]) {
            let list: [Row] = self.collection.array
            let compare: Compare = self.compare
            
            var updated: [(Int, Row)] = []
            var removed: [(Int, Row)] = []
            var remain: [Key: Row] = [:]
            _ = self._dictionary.map { (item) -> Void in
                remain[item.key] = item.value
            }
            
            var array: [Row] = []
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
                    
            var waitInserted: [Row] = remain.map { (item) -> Row in
                return item.value
            }.sorted { (lhs, rhs) -> Bool in
                return compare(lhs.priority, rhs.priority)
            }
            
            if !waitInserted.isEmpty {
                var inserted: [(Int, Row)] = []
                var resultArray: [Row] = []
                
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

    fileprivate var array: [Row]
    fileprivate var dictionary: [Key: Int]

    public var count: Int {
        return self.array.count
    }
    
    public private(set) var order: TableOrder {
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
    public init(order: TableOrder = .ascending) {
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
    public subscript(index: Int) -> Row {
        return self.array[index]
    }
    public subscript(key: Key) -> Row? {
        guard let index = self.dictionary[key] else {
            return nil
        }
        return self.array[index]
    }
    public func index(of key: Key) -> Int? {
        return self.dictionary[key]
    }
}


public class PriorityCollection<Key, Priority, Value> where Key: Hashable, Priority: Comparable, Value: Equatable {

    public typealias Storage = Table<Key, Priority, Value>
    public typealias Updater = Storage.Updater
    public typealias Change = TableChange<Storage.Row>
    public typealias ObserverClosure = (_ entity: PriorityCollection<Key, Priority, Value>, _ changes: [Change]) -> Void

    public private(set) var storage: Storage

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public private(set) var order: TableOrder

    public init(order: TableOrder = .ascending) {
        self.order = order
        self.storage = Storage(order: order)
    }

    public func observeList(didChange: @escaping ObserverClosure, forKey: AnyHashable) {
        self.observers[forKey] = didChange
    }
    public func unobserveList(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    public func update(_ body: (_ updater: inout Updater) -> Void) {
        var updater = self.storage.makeUpdater()
        body(&updater)
        let t = CFAbsoluteTimeGetCurrent()
        let (storage, changes) = updater.finish()
        self.storage = storage
        let e = CFAbsoluteTimeGetCurrent()
        print("finish used: \(e-t)")
        if changes.count > 0 {
            let observers = self.observers
            _ = observers.mapValues { (body) -> Void in
                body(self, changes)
            }
        }
    }
}
