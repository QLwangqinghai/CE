//
//  UniqueOrderedList.swift
//  TTable
//
//  Created by vector on 2019/10/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum ListChange<Element: Any> {
    case remove([(Int, Element)])
    case insert([(Int, Element)])
    case move([(Int, Int, Element)])
}

public protocol UniqueElement {
    associatedtype UniqueIdentifier: Hashable
    var uniqueIdentifier: UniqueIdentifier {
        get
    }
    func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String)
    func unobserveContent(forKey:String)
}

public class UniqueOrderedList<Order, Value> where Value: UniqueElement, Order: Comparable {
    public typealias Key = Value.UniqueIdentifier
    
    private class Element<Order, Value> {
        let row: UInt64
        let key: Key
        var index: Int
        var order: Order
        var value: Value
        init(row: UInt64, key: Key, order: Order, value: Value) {
            self.row = row
            self.order = order
            self.key = key
            self.value = value
        }
    }
    
    
    private var dictionary: [Key: Element<Order, Value>] = [:]
    

    
    public struct Updater<Order, Value> where Value: UniqueElement, Order: Comparable {
        public typealias Key = Value.UniqueIdentifier
        
        public private(set) var dictionary: [Key: (Order, Value)]
        public init(dictionary: [Key: (Order, Value)]) {
            self.dictionary = dictionary
        }
        public mutating func filter(_ body: (Order, Value) -> Bool) {
            self.dictionary = self.dictionary.filter({ (item) -> Bool in
                return body(item.value.0, item.value.1)
            })
        }
        public mutating func replace<S>(_ items: S) where (Order, Value) == S.Element, S : Sequence {
            for item in items {
                self.dictionary[item.1.uniqueIdentifier] = item
            }
        }
        public mutating func removeAll() {
            self.dictionary.removeAll()
        }
    }

    
    public typealias Change = ListChange<(Order, Value)>
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Order, Value>, _ changes: [Change]) -> Void

    private var orderedArray: OrderedArray<Order, Value>
    private var dictionary: [Key: (Order, Value)]

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public init(order: ListOrder = .ascending) {
        self.orderedArray = OrderedArray<Order, Value>(order: order)
        self.dictionary = [:]
    }
    
    public func load() -> [Value] {
        return self.orderedArray.array.map { (item) -> Value in
            return item.1
        }
    }
    
    public func observeList(didChange: @escaping ObserverClosure, forKey: AnyHashable) {
        self.observers[forKey] = didChange
    }
    public func unobserveList(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    private func _update(dictionary: [Key: (Order, Value)]) {
        let oldDictionary = self.dictionary
        var waitRemoved: Set<Key> = []
        var orderChanged: [Key: (Order, Value)] = [:]
        var remain = dictionary
        for (key, item) in oldDictionary {
            if let newItem = dictionary[key] {
                if newItem.0 != item.0 {
                    orderChanged[newItem.1.uniqueIdentifier] = newItem
                }
                remain.removeValue(forKey: key)
            } else {
                waitRemoved.insert(key)
            }
        }
        let waitInserted: [(Order, Value)] = remain.map { (item) -> (Order, Value) in
            return item.value
        }
        var orderedArray = self.orderedArray
        let removed = orderedArray.filter { (order, value) -> Bool in
            return !waitRemoved.contains(value.uniqueIdentifier)
        }
        orderedArray.replaceValues { (order, value) -> Value in
            return dictionary[value.uniqueIdentifier]?.1 ?? value
        }
        
        let moved = orderedArray.updateOrder { (order, value) -> Order in
            return orderChanged[value.uniqueIdentifier]?.0 ?? order
        }
        print("moved: \(moved)")
        
        let inserted = orderedArray.insertItems(waitInserted)
        self.orderedArray = orderedArray
        self.dictionary = dictionary
        
        var changes: [Change] = []
        if !removed.isEmpty {
            let change = Change.remove(removed.map({ (item) -> (Int, (Order, Value)) in
                return (item.0, (item.1, item.2))
            }))
            changes.append(change)
        }
        if !moved.isEmpty {
            let change = Change.move(moved)
            changes.append(change)
        }
        if !inserted.isEmpty {
            let change = Change.insert(inserted.map({ (item) -> (Int, (Order, Value)) in
                return (item.0, (item.1, item.2))
            }))
            changes.append(change)
        }
        if !changes.isEmpty {
            let observers = self.observers
            for (_, body) in observers {
                body(self, changes)
            }
        }
    }
    
    public func update(_ body: (_ updater: inout Updater<Order, Value>) -> Void) {
        var updater = Updater<Order, Value>(dictionary: self.dictionary)
        body(&updater)
        self._update(dictionary: updater.dictionary)
    }
}
