//
//  UniqueOrderedList.swift
//  TTable
//
//  Created by vector on 2019/10/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum ArrayOrder {
    case ascending
    case descending
}

public enum ListChange<Element: Any> {
    case remove([(Int, Element)])//(oldIndex, (oldOrder, value))
    case insert([(Int, Element)])//(index, (order, value))
}

public protocol UniqueValue {
    associatedtype UniqueIdentifier: Hashable
    var uniqueIdentifier: UniqueIdentifier {
        get
    }
}

public struct UniqueOrderedArray<Order, Value> where Order: Comparable, Value: UniqueValue {
    public typealias Key = Value.UniqueIdentifier
    
    public struct Element {
        public let order: Order
        public let value: Value
        public init(order: Order, value: Value) {
            self.order = order
            self.value = value
        }
    }
    
    public struct Updater {
        fileprivate let collection: UniqueOrderedArray<Order, Value>
        fileprivate var _dictionary: [Key: Element]
        private(set) var compare: (_ lhs: Order, _ rhs: Order) -> Bool

        fileprivate init(collection: UniqueOrderedArray<Order, Value>, compare: @escaping (_ lhs: Order, _ rhs: Order) -> Bool) {
            self.compare = compare
            self.collection = collection
            self._dictionary = collection.dictionary.mapValues({ (index) -> Element in
                let item = collection.array[index]
                return Element(order: item.order, value: item.value)
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
        public mutating func replaceItem(_ item: Element) {
            let key = item.value.uniqueIdentifier
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
        public mutating func replace<S>(_ items: S) where Element == S.Element, S : Sequence {
            for item in items {
                let key = item.value.uniqueIdentifier
                self._dictionary[key] = item
            }
        }
                
        public mutating func removeAll() {
            self._dictionary.removeAll()
        }
        
        public mutating func finish() -> (UniqueOrderedArray<Order, Value>, [ListChange<Element>]) {
            let list: [Element] = self.collection.array
            let compare: (_ lhs: Order, _ rhs: Order) -> Bool = self.compare
            
            var removed: [(Int, Element)] = []
            var remain: [Key: Element] = [:]
            _ = self._dictionary.map { (item) -> Void in
                remain[item.value.value.uniqueIdentifier] = item.value
            }
            
            var array: [Element] = []
            var dictionary: [Key: Int] = [:]
            var changes: [ListChange<Element>] = []
            
            //remove
            for (index, arrayItem) in list.enumerated() {
                let key = arrayItem.value.uniqueIdentifier
                if let newItem = remain[key] {
                    if arrayItem.order == newItem.order {
                        remain.removeValue(forKey: key)
                        array.append(newItem)
                    } else {
                        removed.append((index, arrayItem))
                    }
                } else {
                    removed.append((index, arrayItem))
                }
            }
            if !removed.isEmpty {
                let change = ListChange<Element>.remove(removed)
                changes.append(change)
            }
                    
            var waitInserted: [Element] = remain.map { (item) -> Element in
                return item.value
            }.sorted { (lhs, rhs) -> Bool in
                return compare(lhs.order, rhs.order)
            }
            
            if !waitInserted.isEmpty {
                var inserted: [(Int, Element)] = []
                var resultArray: [Element] = []
                
                for item in array {
                    while let insertItem = waitInserted.first {
                        if compare(insertItem.order, item.order) {
                            let insertIndex = resultArray.endIndex
                            resultArray.append(insertItem)
                            inserted.append((insertIndex, insertItem))
                            dictionary[insertItem.value.uniqueIdentifier] = insertIndex
                            waitInserted.removeFirst()
                        } else {
                            break
                        }
                    }
                    let insertIndex = resultArray.endIndex
                    resultArray.append(item)
                    dictionary[item.value.uniqueIdentifier] = insertIndex
                }
                
                while let insertItem = waitInserted.first {
                    let insertIndex = resultArray.endIndex
                    resultArray.append(insertItem)
                    inserted.append((insertIndex, insertItem))
                    dictionary[insertItem.value.uniqueIdentifier] = insertIndex
                    waitInserted.removeFirst()
                }
                
                array = resultArray
                if !inserted.isEmpty {
                    let change = ListChange<Element>.insert(inserted)
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
    
    public private(set) var order: ArrayOrder {
        didSet {
            let _order = self.order
            self.compare = { (_ lhs: Order, _ rhs: Order) -> Bool in
                switch _order {
                case .ascending:
                    return lhs < rhs
                case .descending:
                    return lhs > rhs
                }
            }
        }
    }
    private(set) var compare: (_ lhs: Order, _ rhs: Order) -> Bool
    public init(order: ArrayOrder = .ascending) {
        self.order = order
        self.array = []
        self.dictionary = [:]
        
        self.compare = { (_ lhs: Order, _ rhs: Order) -> Bool in
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

public class UniqueOrderedList<Order, Value> where Order: Comparable, Value: UniqueValue {
    public typealias Key = Value.UniqueIdentifier
    
    public typealias Updater = UniqueOrderedArray<Order, Value>.Updater

    public typealias Storage = UniqueOrderedArray<Order, Value>
    public typealias Change = ListChange<Storage.Element>
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Order, Value>, _ changes: [Change]) -> Void

    public private(set) var storage: Storage

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public private(set) var order: ArrayOrder
    
    public init(order: ArrayOrder = .ascending) {
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
