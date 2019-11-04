//
//  OrderedDictionary.swift
//  Asset
//
//  Created by vector on 2019/11/1.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public struct OrderedDictionary<Key, Value> where Key: Hashable {
    public struct Element {
        public var key: Key
        public let value: Value
        public init(key: Key, value: Value) {
            self.key = key
            self.value = value
        }
    }
    
    public typealias Change = ListChange<Element>
    
    
//    let a: BidirectionalCollection
    
//    public struct Updater {
//        public typealias Collection = UniqueOrderedArray<Order, Value>
//
//        fileprivate let collection: Collection
//        fileprivate var _dictionary: [Key: Element]
//        private(set) var compare: (_ lhs: Order, _ rhs: Order) -> Bool
//
//        fileprivate init(collection: Collection, compare: @escaping (_ lhs: Order, _ rhs: Order) -> Bool) {
//            self.compare = compare
//            self.collection = collection
//            self._dictionary = collection.dictionary.mapValues({ (index) -> Element in
//                let item = collection.array[index]
//                return Element(order: item.order, value: item.value)
//            })
//        }
//
//        public func uniqueValue(forKey: Key) -> Element? {
//            if let result = self._dictionary[forKey] {
//                return result
//            }
//            return self.collection[forKey]
//        }
//
//        public mutating func removeItem(forKey: Key) {
//            self._dictionary.removeValue(forKey: forKey)
//        }
//        public mutating func replaceItem(_ item: Element) {
//            self._dictionary[item.identifier] = item
//        }
//
//        public mutating func filter(_ body: (Element) -> Bool) {
//            self._dictionary = self._dictionary.filter({ (item) -> Bool in
//                return body(item.value)
//            })
//        }
//        public mutating func reset(_ dictionary: [Key: Element]) {
//            self._dictionary = dictionary
//        }
//        public mutating func replace<S>(_ items: S) where Element == S.Element, S : Sequence {
//            for item in items {
//                self._dictionary[item.identifier] = item
//            }
//        }
//
//        public mutating func removeAll() {
//            self._dictionary.removeAll()
//        }
//
//        public mutating func finish() -> (Collection, [Change]) {
//            let list: [Element] = self.collection.array
//            let compare: (_ lhs: Order, _ rhs: Order) -> Bool = self.compare
//
//            var removed: [(Int, Element)] = []
//            var remain: [Key: Element] = [:]
//            _ = self._dictionary.map { (item) -> Void in
//                remain[item.value.identifier] = item.value
//            }
//
//            var array: [Element] = []
//            var dictionary: [Key: Int] = [:]
//            var changes: [Change] = []
//
//            //remove
//            for (index, arrayItem) in list.enumerated() {
//                let key = arrayItem.identifier
//                if let newItem = remain[key] {
//                    if arrayItem.order == newItem.order {
//                        remain.removeValue(forKey: key)
//                        array.append(newItem)
//                    } else {
//                        removed.append((index, arrayItem))
//                    }
//                } else {
//                    removed.append((index, arrayItem))
//                }
//            }
//            if !removed.isEmpty {
//                let change = Change.remove(removed)
//                changes.append(change)
//            }
//
//            var waitInserted: [Element] = remain.map { (item) -> Element in
//                return item.value
//            }.sorted { (lhs, rhs) -> Bool in
//                return compare(lhs.order, rhs.order)
//            }
//
//            if !waitInserted.isEmpty {
//                var inserted: [(Int, Element)] = []
//                var resultArray: [Element] = []
//
//                for item in array {
//                    while let insertItem = waitInserted.first {
//                        if compare(insertItem.order, item.order) {
//                            let insertIndex = resultArray.endIndex
//                            resultArray.append(insertItem)
//                            inserted.append((insertIndex, insertItem))
//                            dictionary[insertItem.identifier] = insertIndex
//                            waitInserted.removeFirst()
//                        } else {
//                            break
//                        }
//                    }
//                    let insertIndex = resultArray.endIndex
//                    resultArray.append(item)
//                    dictionary[item.identifier] = insertIndex
//                }
//
//                while let insertItem = waitInserted.first {
//                    let insertIndex = resultArray.endIndex
//                    resultArray.append(insertItem)
//                    inserted.append((insertIndex, insertItem))
//                    dictionary[insertItem.identifier] = insertIndex
//                    waitInserted.removeFirst()
//                }
//
//                array = resultArray
//                if !inserted.isEmpty {
//                    let change = Change.insert(inserted)
//                    changes.append(change)
//                }
//            }
//            var storage = self.collection
//            storage.array = array
//            storage.dictionary = dictionary
//            return (storage, changes)
//        }
//    }
//
//    public func makeUpdater() -> Updater {
//        return Updater(collection: self, compare: self.compare)
//    }

    fileprivate var array: [Element]
    fileprivate var dictionary: [Key: Int]

    public var count: Int {
        return self.array.count
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

public struct PriorityDictionary<Key, Priority, Value> where Key: Hashable, Priority: Comparable, Value: Equatable {
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
        public typealias Collection = PriorityDictionary<Key, Priority, Value>
        
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


public class PriorityCollection<Key, Priority, Value> where Key: Hashable, Priority: Comparable, Value: Equatable {

    public typealias Storage = PriorityDictionary<Key, Priority, Value>
    public typealias Updater = Storage.Updater
    public typealias Change = CollectionChange<Int, Storage.Element>
    public typealias ObserverClosure = (_ entity: PriorityCollection<Key, Priority, Value>, _ changes: [Change]) -> Void

    public private(set) var storage: Storage

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public private(set) var order: Order

    public init(order: Order = .ascending) {
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
