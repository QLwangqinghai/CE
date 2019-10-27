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
    func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String)
    func unobserveContent(forKey:String)
}

public struct UniqueOrderedCollection<Order, Value> where Order: Comparable, Value: UniqueElement {
    public typealias Key = Value.UniqueIdentifier
    
    public struct Updater {
        fileprivate let collection: UniqueOrderedCollection<Order, Value>
        fileprivate var _dictionary: [Key: (Order, Value)]
        private(set) var compare: (_ lhs: Order, _ rhs: Order) -> Bool

        fileprivate init(collection: UniqueOrderedCollection<Order, Value>, compare: @escaping (_ lhs: Order, _ rhs: Order) -> Bool) {
            self.compare = compare
            self.collection = collection
            self._dictionary = collection.dictionary.mapValues({ (cell) -> (Order, Value) in
                return (cell.order, cell.value)
            })
        }
        
        public func oldItem(forKey: Key) -> (Order, Value)? {
            guard let cell = self.collection.dictionary[forKey] else {
                return nil
            }
            return (cell.order, cell.value)
        }
        public func item(forKey: Key) -> (Order, Value)? {
            return self._dictionary[forKey]
        }
        
        public mutating func removeItem(forKey: Key) {
            self._dictionary.removeValue(forKey: forKey)
        }
        public mutating func replaceItem(value: Value, order: Order) {
            let key = value.uniqueIdentifier
            self._dictionary[key] = (order, value)
        }
        
        public mutating func filter(_ body: (Order, Value) -> Bool) {
            self._dictionary = self._dictionary.filter({ (item) -> Bool in
                return body(item.value.0, item.value.1)
            })
        }
        public mutating func reset(_ dictionary: [Key: (Order, Value)]) {
            self._dictionary = dictionary
        }
        public mutating func replace<S>(_ items: S) where (Order, Value) == S.Element, S : Sequence {
            for item in items {
                let key = item.1.uniqueIdentifier
                self._dictionary[key] = item
            }
        }
                
        public subscript(key: Key) -> Value? {
            return self._dictionary[key]?.1
        }
        public mutating func removeAll() {
            self._dictionary.removeAll()
        }
        
        public mutating func finish() -> (UniqueOrderedCollection<Order, Value>, [ListChange<(Order, Value)>]) {
            let list: [(Order, Value)] = self.collection.array
            let compare: (_ lhs: Order, _ rhs: Order) -> Bool = self.compare
            
            var removed: [(Int, (Order, Value))] = []
            var remain: [Key: (Order, Value)] = [:]
            _ = self._dictionary.map { (item) -> Void in
                remain[item.value.1.uniqueIdentifier] = item.value
            }
            
            var array: [(Order, Value)] = []
            var dictionary: [Key: _Cell] = [:]
            var changes: [ListChange<(Order, Value)>] = []
            
            //remove
            for (index, arrayItem) in list.enumerated() {
                let (order, value) = arrayItem
                let key = value.uniqueIdentifier
                if let newItem = remain[key] {
                    if order == newItem.0 {
                        remain.removeValue(forKey: key)
                        array.append((order, newItem.1))
                    } else {
                        removed.append((index, arrayItem))
                    }
                } else {
                    removed.append((index, arrayItem))
                }
            }
            if !removed.isEmpty {
                let change = ListChange<(Order, Value)>.remove(removed)
                changes.append(change)
            }
                    
            var waitInserted: [(Order, Value)] = remain.map { (item) -> (Order, Value) in
                return item.value
            }.sorted { (lhs, rhs) -> Bool in
                return compare(lhs.0, rhs.0)
            }
            
            if !waitInserted.isEmpty {
                var inserted: [(Int, (Order, Value))] = []
                var resultArray: [(Order, Value)] = []
                
                for item in array {
                    while let insertItem = waitInserted.first {
                        if compare(insertItem.0, item.0) {
                            let insertIndex = resultArray.endIndex
                            resultArray.append(insertItem)
                            inserted.append((insertIndex, insertItem))
                            dictionary[insertItem.1.uniqueIdentifier] = _Cell(index: insertIndex, order: insertItem.0, value: insertItem.1)
                            waitInserted.removeFirst()
                        } else {
                            break
                        }
                    }
                    let insertIndex = resultArray.endIndex
                    resultArray.append(item)
                    dictionary[item.1.uniqueIdentifier] = _Cell(index: insertIndex, order: item.0, value: item.1)
                }
                
                while let insertItem = waitInserted.first {
                    let insertIndex = resultArray.endIndex
                    resultArray.append(insertItem)
                    inserted.append((insertIndex, insertItem))
                    dictionary[insertItem.1.uniqueIdentifier] = _Cell(index: insertIndex, order: insertItem.0, value: insertItem.1)
                    waitInserted.removeFirst()
                }
                
                array = resultArray
                if !inserted.isEmpty {
                    let change = ListChange<(Order, Value)>.insert(inserted)
                    changes.append(change)
                }
            }
            var storage = self.collection
            storage.array = array
            storage.dictionary = dictionary
            return (storage, changes)
        }
        
//        public func update<S>(to content: S) -> (UniqueOrderedCollection<Order, Value>, [ListChange<(Order, Value)>])  where (Order, Value) == S.Element, S : Sequence {
//
//            let list: [(Order, Value)] = self.collection.array
//            let compare: (_ lhs: Order, _ rhs: Order) -> Bool = self.compare
//
//            var removed: [(Int, (Order, Value))] = []
//            var remain: [Key: (Order, Value)] = [:]
//            _ = content.map { (item) -> Void in
//                remain[item.1.uniqueIdentifier] = item
//            }
//
//            var array: [(Order, Value)] = []
//            var dictionary: [Key: _Cell] = [:]
//            var changes: [ListChange<(Order, Value)>] = []
//
//            //remove
//            for (index, arrayItem) in list.enumerated() {
//                let (order, value) = arrayItem
//                let key = value.uniqueIdentifier
//                if let newItem = remain[key] {
//                    if order == newItem.0 {
//                        remain.removeValue(forKey: key)
//                        array.append((order, newItem.1))
//                    } else {
//                        removed.append((index, arrayItem))
//                    }
//                } else {
//                    removed.append((index, arrayItem))
//                }
//            }
//            if !removed.isEmpty {
//                let change = ListChange<(Order, Value)>.remove(removed)
//                changes.append(change)
//            }
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
//                    let change = ListChange<(Order, Value)>.insert(inserted)
//                    changes.append(change)
//                }
//            }
//            var storage = self.collection
//            storage.array = array
//            storage.dictionary = dictionary
//            return (storage, changes)
//        }
    }
    
    public func makeUpdater() -> Updater {
        return Updater(collection: self, compare: self.compare)
    }
    
    fileprivate struct _Cell {
        var index: Int
        var order: Order
        var value: Value
        init(index: Int, order: Order, value: Value) {
            self.index = index
            self.order = order
            self.value = value
        }
    }
    fileprivate var array: [(Order, Value)]
    fileprivate var dictionary: [Key: _Cell]

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
            return item.1
        }
    }
    public func object(at index: Int) -> Value {
        return self.array[index].1
    }
    public subscript(key: Key) -> Value? {
        return self.dictionary[key]?.value
    }
    public func index(of key: Key) -> Int? {
        return self.dictionary[key]?.index
    }
    
    
//    public mutating func replace<S>(_ items: S) where (Order, Value) == S.Element, S : Sequence {

}

public class UniqueOrderedList<Order, Value> where Order: Comparable, Value: UniqueElement {
    public typealias Key = Value.UniqueIdentifier
    
    public typealias Updater = UniqueOrderedCollection<Order, Value>.Updater

//    public struct Updater {
//        fileprivate var dictionary: [Key: (Order, Value)]
//        fileprivate init(dictionary: [Key: (Order, Value)]) {
//            self.dictionary = dictionary
//        }
//
//        public mutating func filter(_ body: (Order, Value) -> Bool) {
//            self.dictionary = self.dictionary.filter({ (item) -> Bool in
//                return body(item.value.0, item.value.1)
//            })
//        }
//        public mutating func replace<S>(_ items: S) where (Order, Value) == S.Element, S : Sequence {
//            for item in items {
//                let key = item.1.uniqueIdentifier
//                self.dictionary[key] = item
//            }
//        }
//        public subscript(key: Key) -> Value? {
//            return self.dictionary[key]?.1
//        }
//        public mutating func removeAll() {
//            self.dictionary.removeAll()
//        }
//    }

    
    public typealias Change = ListChange<(Order, Value)>
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Order, Value>, _ changes: [Change]) -> Void
    public typealias Storage = UniqueOrderedCollection<Order, Value>

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
//        var updater = self.storage.makeUpdater()
//        body(&updater)
//
//
//
//        var dictionary: [Key: (Order, Value)] = [:]
//        _ = self.storage.dictionary.map({ (item) -> Void in
//            dictionary[item.key] = (item.value.order, item.value.value)
//        })
//        var updater = Updater(dictionary: dictionary)
//        body(&updater)
//        let (storage, changes) = self.storage.reset(by: updater.dictionary)
//        self.storage = storage
//        if changes.count > 0 {
//            let observers = self.observers
//            _ = observers.mapValues { (body) -> Void in
//                body(self, changes)
//            }
//        }
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

