//
//  UniqueOrderedList.swift
//  TTable
//
//  Created by vector on 2019/10/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum ListOrder {
    case ascending
    case descending
}

public enum ListChange<Element: Any> {
    case remove([(Int, Element)])//(oldIndex, (oldOrder, value))
    case insert([(Int, Element)])//(index, (order, value))
}

public protocol UniqueElement {
    associatedtype UniqueIdentifier: Hashable
    var uniqueIdentifier: UniqueIdentifier {
        get
    }
}

public struct UniqueOrderedCollection<Order, Value> where Order: Comparable, Value: UniqueElement {
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
        fileprivate let collection: UniqueOrderedCollection<Order, Value>
        fileprivate var _dictionary: [Key: Element]
        private(set) var compare: (_ lhs: Order, _ rhs: Order) -> Bool

        fileprivate init(collection: UniqueOrderedCollection<Order, Value>, compare: @escaping (_ lhs: Order, _ rhs: Order) -> Bool) {
            self.compare = compare
            self.collection = collection
            self._dictionary = collection.dictionary.mapValues({ (index) -> Element in
                let item = collection.array[index]
                return Element(order: item.order, value: item.value)
            })
        }
        
        public func oldItem(forKey: Key) -> Element? {
            return self.collection[forKey]
        }
        public func item(forKey: Key) -> Element? {
            return self._dictionary[forKey]
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
                
        public subscript(key: Key) -> Element? {
            return self._dictionary[key]
        }
        public mutating func removeAll() {
            self._dictionary.removeAll()
        }
        
        public mutating func finish() -> (UniqueOrderedCollection<Order, Value>, [ListChange<Element>]) {
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
    
    public private(set) var order: ListOrder {
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
    public init(order: ListOrder = .ascending) {
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

public class UniqueOrderedList<Order, Value> where Order: Comparable, Value: UniqueElement {
    public typealias Key = Value.UniqueIdentifier
    
    public typealias Updater = UniqueOrderedCollection<Order, Value>.Updater

    public typealias Storage = UniqueOrderedCollection<Order, Value>
    public typealias Change = ListChange<Storage.Element>
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Order, Value>, _ changes: [Change]) -> Void

    public private(set) var storage: Storage

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public private(set) var order: ListOrder
    
    public init(order: ListOrder = .ascending) {
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
//        public func _updated(by content: [Key: (Order, Value)]) -> (Storage, [Change]) {
//            let list: [(Order, Value)] = self.array
//            let compare: (_ lhs: Order, _ rhs: Order) -> Bool = self.compare
//
//            let goodOrder: Bool = {
//                guard list.count >= 2 else {
//                    return true
//                }
//                let endIndex = list.count - 2
//                for index in 0 ... endIndex {
//                    if !compare(list[index].0, list[index+1].0) {
//                        return false
//                    }
//                }
//                return true
//            }()
//
//            var removed: [(Int, (Order, Value))] = []
//            var orderChanged: [Key: (Order, Value)] = [:]
//            var remain = content
//
//            var array: [(Order, Value)] = []
//            var dictionary: [Key: _Cell] = [:]
//            var changes: [Change] = []
//
//            //remove
//            for (index, arrayItem) in list.enumerated() {
//                let (order, value) = arrayItem
//                let key = value.uniqueIdentifier
//                if let newItem = remain[key] {
//                    if goodOrder {
//                        if order != newItem.0 {
//                            orderChanged[key] = newItem
//                        }
//                        remain.removeValue(forKey: key)
//                        array.append((order, newItem.1))
//                    } else {
//                        if order == newItem.0 {
//                            remain.removeValue(forKey: key)
//                            array.append((order, newItem.1))
//                        } else {
//                            removed.append((index, arrayItem))
//                        }
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
//            func updateOrder(at index: Int, newOrder: Order, array: inout [(Order, Value)], indexMap: inout [Key: Int]) -> (Order, Int?) {
//                func goodInsertIndex(order: Order, start: Int, end: Int) -> Int {
//                    if end - start <= 0 {
//                        return end
//                    }
//                    for index in start ... end {
//                        if compare(order, array[index].0) {
//                            return index
//                        }
//                    }
//                    return end
//                }
//                let old = array[index]
//                guard old.0 != newOrder else {
//                    return (old.0, nil)
//                }
//                array[index] = (newOrder, old.1)
//                guard array.count > 1 else {
//                    return (old.0, nil)
//                }
//
//                if index == array.startIndex {
//                    if compare(array[index].0, array[index + 1].0) {
//                        return (old.0, nil)
//                    }
//                } else if index == array.endIndex-1 {
//                    if compare(array[index-1].0, array[index].0) {
//                        return (old.0, nil)
//                    }
//                } else {
//                    if compare(array[index-1].0, array[index].0) && compare(array[index].0, array[index + 1].0) {
//                        return (old.0, nil)
//                    }
//                }
//
//                array.remove(at: index)
//                let insertIndex: Int
//                if compare(old.0, newOrder) {//right side insert
//                    insertIndex = goodInsertIndex(order: newOrder, start: index, end: array.count - 1)
//                } else {//left side insert
//                    insertIndex = goodInsertIndex(order: newOrder, start: 0, end: index - 1)
//                }
//                array.insert((newOrder, old.1), at: insertIndex)
//
//                if insertIndex < index {
//                    for i in insertIndex ... index {
//                        indexMap[array[i].1.uniqueIdentifier] = i
//                    }
//                } else if insertIndex > index {
//                    for i in index ... insertIndex {
//                        indexMap[array[i].1.uniqueIdentifier] = i
//                    }
//                }
//                return (old.0, insertIndex)
//            }
//            //resort
//            if !orderChanged.isEmpty {
//                var moved: [Change.MoveInfo] = []
//
//                var indexMap: [Key: Int] = [:]
//                for (index, arrayItem) in array.enumerated() {
//                    indexMap[arrayItem.1.uniqueIdentifier] = index
//                }
//
//                for (key, item) in orderChanged {
//                    let index = indexMap[key]!
//
//                    let (oldOrder, newIndex) = updateOrder(at: index, newOrder: item.0, array: &array, indexMap: &indexMap)
//                    if let newIndexValue = newIndex {
//                        moved.append(Change.MoveInfo(old: (index, (oldOrder, item.1)), new: (newIndexValue, item)))
//                    }
//                }
//                if !moved.isEmpty {
//                    let change = Change.move(moved)
//                    changes.append(change)
//                }
//            }
//
//
//            var waitInserted: [(Order, Value)] = remain.map { (item) -> (Order, Value) in
//                return item.value
//            }.sorted { (lhs, rhs) -> Bool in
//                return compare(lhs.0, rhs.0)
//            }
//
//            if !waitInserted.isEmpty {
//                var inserted: [(Int, (Order, Value))] = []
//                var resultArray: [(Order, Value)] = []
//
//                for item in array {
//                    while let insertItem = waitInserted.first {
//                        if compare(insertItem.0, item.0) {
//                            let insertIndex = resultArray.endIndex
//                            resultArray.append(insertItem)
//                            inserted.append((insertIndex, insertItem))
//                            dictionary[insertItem.1.uniqueIdentifier] = _Cell(index: insertIndex, order: insertItem.0, value: insertItem.1)
//                            waitInserted.removeFirst()
//                        } else {
//                            break
//                        }
//                    }
//                    let insertIndex = resultArray.endIndex
//                    resultArray.append(item)
//                    dictionary[item.1.uniqueIdentifier] = _Cell(index: insertIndex, order: item.0, value: item.1)
//                }
//
//                while let insertItem = waitInserted.first {
//                    let insertIndex = resultArray.endIndex
//                    resultArray.append(insertItem)
//                    inserted.append((insertIndex, insertItem))
//                    dictionary[insertItem.1.uniqueIdentifier] = _Cell(index: insertIndex, order: insertItem.0, value: insertItem.1)
//                    waitInserted.removeFirst()
//                }
//
//                array = resultArray
//                if !inserted.isEmpty {
//                    let change = Change.insert(inserted)
//                    changes.append(change)
//                }
//            }
//            var storage = self
//            storage.array = array
//            storage.dictionary = dictionary
//            return (storage, changes)
//        }

