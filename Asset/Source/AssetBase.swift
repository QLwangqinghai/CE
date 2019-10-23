//
//  AssetBase.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation


public enum ListChange<Element: Any> {
    case remove([(Int, Element)])
    case insert([(Int, Element)])
}

public struct AssetOrder: Comparable {
    //主维度
    public var main: Int64
    
    //二级维度
    public var time: Int64
    
    //三级维度
    public var sequence: Int64
    
    public init(main: Int64, time: Int64, sequence: Int64) {
        self.main = main
        self.time = time
        self.sequence = sequence
    }
    public static func < (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedAscending
    }
    public static func > (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedDescending
    }
    public static func <= (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) != .orderedDescending
    }
    public static func >= (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) != .orderedAscending
    }
    public static func == (lhs: AssetOrder, rhs: AssetOrder) -> Bool {
        return self._compare(lhs: lhs, rhs: rhs) == .orderedSame
    }
    private static func _compare(lhs: AssetOrder, rhs: AssetOrder) -> ComparisonResult {
        if lhs.main < rhs.main {
            return .orderedAscending
        } else if lhs.main > rhs.main {
            return .orderedDescending
        }
        if lhs.time < rhs.time {
            return .orderedAscending
        } else if lhs.time > rhs.time {
            return .orderedDescending
        }
        if lhs.sequence < rhs.sequence {
            return .orderedAscending
        } else if lhs.sequence > rhs.sequence {
            return .orderedDescending
        }
        return .orderedSame
    }
}

public protocol UniqueOrderedListElement {
    associatedtype UniqueOrder: Comparable
    associatedtype UniqueIdentifier: Hashable

    var uniqueOrder: UniqueOrder {
        get
    }
    var uniqueIdentifier: UniqueIdentifier {
        get
    }
    
    func observeUniqueOrder(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String)
    func unobserveUniqueOrder(forKey:String)

    func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String)
    func unobserveContent(forKey:String)
}

public enum ListOrder {
    case ascending
    case escending
}

public class UniqueOrderedList<Value> where Value: UniqueOrderedListElement & Equatable {
    public typealias Key = Value.UniqueIdentifier
    public typealias Order = Value.UniqueOrder

    public typealias ReloadClosure = (_ entity: UniqueOrderedList<Value>, _ changes: [(Int, Value)]) -> Void
    public typealias ChangeClosure = (_ entity: UniqueOrderedList<Value>, _ changes: [ListChange<Value>]) -> Void
    public typealias ReloadAllClosure = (_ entity: UniqueOrderedList<Value>) -> Void
    
    private struct _Observer {
        public let didChange: ChangeClosure
        public let didReload: ReloadClosure
        public let didReloadAll: ReloadAllClosure

        public init(didChange: @escaping ChangeClosure, didReload: @escaping ReloadClosure, didReloadAll: @escaping ReloadAllClosure) {
            self.didReload = didReload
            self.didChange = didChange
            self.didReloadAll = didReloadAll
        }
    }
    
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Value>, _ changes: [ListChange<Value>]) -> Void
    public typealias Change = ListChange<Value>


    public private(set) var list: [Value] = []
    
    //主维度
    public private(set) var dictionary: [Key: Value] = [:]

    public private(set) var order: ListOrder
    private let orderObserveKey: String = UUID().uuidString
    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: _Observer] = [:]
    public init(order: ListOrder = .ascending) {
        self.order = order
    }
    
    public func observeList(didChange: @escaping ChangeClosure, didReload: @escaping ReloadClosure, didReloadAll: @escaping ReloadAllClosure, forKey: AnyHashable) {
        self.observers[forKey] = _Observer(didChange: didChange, didReload: didReload, didReloadAll: didReloadAll)
    }
    public func unobserveList(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    public func reloadAll() {
        self.list = self.dictionary.map { (item) -> Value in
            return item.value
        }.sorted { (lhs, rhs) -> Bool in
            return true
        }.map { (item) -> Value in
            return item
        }
        self.didReloadAll()
    }
    private func didReloadAll() {
        let observers = self.observers
        _ = observers.map { (item) -> Void in
            item.value.didReloadAll(self)
        }
    }
    private func _compare(lhs: Value, rhs: Value) -> Bool {
        switch self.order {
        case .ascending:
            return lhs.uniqueOrder < rhs.uniqueOrder
        case .escending:
            return lhs.uniqueOrder > rhs.uniqueOrder
        }
    }
    public func sort() {
        let newList: [Value] = self.dictionary.map { (item) -> Value in
            return item.value
        }.sorted { (lhs, rhs) -> Bool in
            return self._compare(lhs: lhs, rhs: rhs)
        }.map { (item) -> Value in
            return item
        }
        guard newList.count == self.list.count && newList.count > 0 else {
            self.list = newList
            self.didReloadAll()
            return
        }
        let oldList = self.list
        self.list = newList
        var changed = false
        for index in 0 ..< newList.count {
            if newList[index] != oldList[index] {
                changed = true
            }
        }
        if changed {
            self.didReloadAll()
        }
    }
    
    //先删除 后插入
    public func batch(filter: ((Value) -> Bool)?, inserts: [Value], reloadAll: Bool = false) {
        guard !reloadAll else {
            var dictionary: [Key: Value]
            if let closure = filter {
                dictionary = self.dictionary.filter { (item) -> Bool in
                    return closure(item.value)
                }
            } else {
                dictionary = self.dictionary
            }
            if !inserts.isEmpty {
                for item in inserts {
                    dictionary[item.uniqueIdentifier] = item
                }
            }
            self.dictionary = dictionary
            self.reloadAll()
            return
        }
        
        func goodInsertIndex(items: [Value], item: Value) -> Int {
            guard !items.isEmpty else {
                return 0
            }
            for (index, v) in items.enumerated() {
                if self._compare(lhs: item, rhs: v) {
                    return index
                }
            }
            return items.count
        }
        
        func onError() {
            self.reloadAll()
        }
        
        var changes: [Change] = []

        var dictionary: [Key: Value] = self.dictionary
        var newList: [Value] = []
        
        if let closure = filter {
            var changedItems: [(Int, Value)] = []
            for (index, item) in self.list.enumerated() {
                if closure(item)  {
                    newList.append(item)
                } else {
                    dictionary.removeValue(forKey: item.uniqueIdentifier)
                    changedItems.append((index, item))
                }
            }
            if !changedItems.isEmpty {
                let change = Change.remove(changedItems)
                changes.append(change)
            }
        }

        if !inserts.isEmpty {
            var changedItems: [(Int, Value)] = []
            let items = inserts.filter({ (item) -> Bool in
                return dictionary[item.uniqueIdentifier] == nil
            }).sorted { (lhs, rhs) -> Bool in
                return rhs.uniqueOrder < rhs.uniqueOrder
            }
            for item in items {
                let index = goodInsertIndex(items: newList, item: item)
                newList.insert(item, at: index)
                dictionary[item.uniqueIdentifier] = item
                changedItems.append((index, item))
            }
            
            if !changedItems.isEmpty {
                let change = Change.insert(changedItems)
                changes.append(change)
            }
        }
        self.list = newList
        
        var removedDictionary = self.dictionary
        var insertedDictionary = dictionary
        
        let allkey = removedDictionary.map { (item) -> Key in
            return item.key
        }
        for k in allkey {
            if insertedDictionary[k] != nil {
                removedDictionary.removeValue(forKey: k)
                insertedDictionary.removeValue(forKey: k)
            }
        }
        for (_, value) in removedDictionary {
            value.unobserveContent(forKey: self.contentObserveKey)
            value.unobserveUniqueOrder(forKey: self.orderObserveKey)
        }
        self.dictionary = dictionary
        for (_, value) in insertedDictionary {
            value.observeContent(onChanged: { (item) in
                
            }, forKey: self.contentObserveKey)
            value.observeUniqueOrder(onChanged: { (key) in
                
            }, forKey: self.orderObserveKey)
        }
        
        if !changes.isEmpty {
            let observers = self.observers
            for (_, observer) in observers {
                observer.didChange(self, changes)
            }
        }
    }
    
    public func filter(_ body: @escaping (Value) -> Bool) {
        self.batch(filter: body, inserts: [])
    }
    public func insert(items: [Value]) {
        self.batch(filter: nil, inserts: items)
    }
    
    public func item(at index: Int) -> Value {
        return self.list[index]
    }
    public func item(forKey: Key) -> Value? {
        return self.dictionary[forKey]
    }
    
}
