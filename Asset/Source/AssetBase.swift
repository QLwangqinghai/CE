//
//  AssetBase.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation


public enum ListChange<Element: Any> {
    case insert([(Int, Element)])
    case remove([(Int, Element)])
    case move([(Int, Int, Element)])
}

//public struct ListChange<Element: Any> {
//    public enum ChangeType: Int {
//        case insert
//        case remove
//    }
//
//    public let type: ChangeType
//    public let items: [(Int, Element)]
//}

//public struct Order {
//    public var main: UInt64
//    public var originalOrder: UInt
//    public static func < (lhs: Order, rhs: Order) -> Bool {
//        return lhs.main == rhs.main ? lhs.originalOrder < rhs.originalOrder : lhs.main < rhs.main
//    }
//}

public final class ListObserver<ListEntity: AnyObject, Element: Any> {
    public typealias ReloadClosure = (_ entity: ListEntity) -> Void
    public typealias ChangeClosure = (_ entity: ListEntity, _ changes: [ListChange<Element>]) -> Void
    public typealias MoveClosure = (_ entity: ListEntity, _ changes: [(Int, Int, Element)]) -> Void
    public typealias UpdateClosure = (_ entity: ListEntity, _ items: [(Int, Element)]) -> Void

    public let didReload: ReloadClosure
    public let didChange: ChangeClosure
    public let didUpdate: UpdateClosure

    public init(didReload: @escaping ReloadClosure, didChange: @escaping ChangeClosure, didUpdate: @escaping UpdateClosure) {
        self.didReload = didReload
        self.didChange = didChange
        self.didUpdate = didUpdate
    }
}


public struct AssetOrder: Comparable {
    //主维度
    public var main: Int64
    
    //二级维度
    public var sub: Int64
    
    public var detail: String
    
    public init(main: Int64, sub: Int64, detail: String) {
        self.main = main
        self.sub = sub
        self.detail = detail
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
        if lhs.sub < rhs.sub {
            return .orderedAscending
        } else if lhs.sub > rhs.sub {
            return .orderedDescending
        }
        if lhs.detail < rhs.detail {
            return .orderedAscending
        } else if lhs.detail > rhs.detail {
            return .orderedDescending
        }
        return .orderedSame
    }
}

public protocol UniqueOrderedListElement {
    associatedtype UniqueOrder
    associatedtype UniqueIdentifier

    var uniqueOrder: UniqueOrder {
        get
    }
    var uniqueIdentifier: UniqueIdentifier {
        get
    }
}



public struct UniqueOrderedListUpdater<Key, UniqueOrder, Value> where Key : Hashable, UniqueOrder: Comparable, Value: UniqueOrderedListElement {
    typealias UniqueOrder = UniqueOrder

    public var list: UniqueOrderedList<Key, UniqueOrder, Value>
    
    public init(list: UniqueOrderedList<Key, UniqueOrder, Value>) {
        self.list = list
    }
    

    
    
    
    public func prepend(item: Value) {

    }
    
    public func append(item: Value) {
        
    }
    
    
    
}

public class UniqueOrderedList<Key, Order, Value> where Key : Hashable, Order: Comparable, Value: UniqueOrderedListElement {
    typealias UniqueOrder = Order
    typealias UniqueIdentifier = Key

    public typealias Observer = ListObserver<UniqueOrderedList<Key, Order, Value>, Value>
    public typealias Change = ListChange<Value>

    public enum ListOrder {
        case ascending
        case escending
    }
    
    public private(set) var list: [(Key, Order)] = []
    public private(set) var map: [Key: Value] = [:]

    public private(set) var order: ListOrder
    public private(set) var observers: [AnyHashable: Observer] = [:]
    
    public init(order: ListOrder = .ascending) {
        self.order = order
    }
    
    public func addListObserver(_ observer: Observer, forKey: AnyHashable) {
        self.observers[forKey] = observer
    }
    public func removeListObserver(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    public func reload() {
        
    }
    
    public func batch(remove: [Key], inserts: [Value], updates: [Value]) {
        func goodInsertIndex(groups: [AssetGroup], group: AssetGroup) -> Int {
            guard !groups.isEmpty else {
                return 0
            }
            for (index, item) in groups.enumerated() {
                if group.order < item.order {
                    return index
                }
            }
            return groups.count
        }
        
        func onError() {
            self.reload()
            let observers = self.observers
            for (_, observer) in observers {
                observer.didReload(self)
            }
        }
        
        var changes: [Change] = []

        var removed: Set<Key> = Set<Key>()
        _ = remove.map { (item) -> Void in
            removed.insert(item)
        }
        if !removed.isEmpty {
            var map: [Key: Value] = self.map
            var newList: [(Key, Order)] = []
            var changedItems: [(Int, Value)] = []
            for (index, item) in self.list.enumerated() {
                if removed.contains(item.0) {
                    let value = map[item.0]!
                    map.removeValue(forKey: item.0)
                    changedItems.append((index, value))
                } else {
                    newList.append(item)
                }
            }
            self.map = map
            self.list = newList
            if !changedItems.isEmpty {
                let change = Change(type: .remove, items: changedItems)
                changes.append(change)
            }
        }
        
        //heath check
        var orderError = false
        let groupsCountAfterRemove = self.groups.count
        if groupsCountAfterRemove >= 2 {
            for index in 0 ..< self.groups.count - 2 {
                if self.groups[index + 1].order < self.groups[index].order {
                    orderError = true
                    break
                }
            }
        }
        
        if orderError {
            onError()
            return
        }
        
        if !inserted.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            var insertGroups = inserted.map { (item) -> AssetGroup in
                return item.value
            }
            insertGroups.sort { (lhs, rhs) -> Bool in
                return rhs.order < rhs.order
            }
            
            for item in insertGroups {
                if self.groups.firstIndex(of: item) != nil {
                    onError()
                    return
                }
                let index = goodInsertIndex(groups: self.groups, group: item)
                self.groups.insert(item, at: index)
                groups.append((index, item))
            }
            
            if !groups.isEmpty {
                let change = GroupListChange(type: .insert, items: groups)
                changes.append(change)
            }
        }
        if !updated.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            for (index, group) in self.groups.enumerated() {
                if let _ = updated[group.identifier] {
                    groups.append((index, group))
                }
            }
            if !groups.isEmpty {
                let change = GroupListChange(type: .update, items: groups)
                changes.append(change)
            }
        }
        if !changes.isEmpty {
            let observers = self.observers
            for (_, observer) in observers {
                observer.didChange(self, changes)
            }
        }

        
        
        
    }
    public func remove(items: [Key]) {
        self.batch(remove: items, inserts: [], updates: [])
    }
    public func insert(items: [Value]) {
        
    }
    public func update(items: [Value]) {
        
    }
    
    public override func groupsDidChange(removed: [String: AssetGroup], inserted: [String: AssetGroup], updated: [String: AssetGroup]) {
        super.groupsDidChange(removed: removed, inserted: inserted, updated:updated)
        
        func goodInsertIndex(groups: [AssetGroup], group: AssetGroup) -> Int {
            guard !groups.isEmpty else {
                return 0
            }
            for (index, item) in groups.enumerated() {
                if group.order < item.order {
                    return index
                }
            }
            return groups.count
        }
        
        func onError() {
            self.reload()
            let observers = self.observers
            for (_, observer) in observers {
                observer.didReload(self)
            }
        }
        
        var changes: [GroupListChange] = []
        if !removed.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            let oldGroups = self.groups
            var newGroups: [AssetGroup] = []
            for (index, item) in oldGroups.enumerated() {
                if removed[item.identifier] != nil {
                    groups.append((index, item))
                } else {
                    newGroups.append(item)
                }
            }
            self.groups = newGroups
            if !groups.isEmpty {
                let change = GroupListChange(type: .remove, items: groups)
                changes.append(change)
            }
        }
        
        //heath check
        var orderError = false
        let groupsCountAfterRemove = self.groups.count
        if groupsCountAfterRemove >= 2 {
            for index in 0 ..< self.groups.count - 2 {
                if self.groups[index + 1].order < self.groups[index].order {
                    orderError = true
                    break
                }
            }
        }
        
        if orderError {
            onError()
            return
        }
        
        if !inserted.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            var insertGroups = inserted.map { (item) -> AssetGroup in
                return item.value
            }
            insertGroups.sort { (lhs, rhs) -> Bool in
                return rhs.order < rhs.order
            }
            
            for item in insertGroups {
                if self.groups.firstIndex(of: item) != nil {
                    onError()
                    return
                }
                let index = goodInsertIndex(groups: self.groups, group: item)
                self.groups.insert(item, at: index)
                groups.append((index, item))
            }
            
            if !groups.isEmpty {
                let change = GroupListChange(type: .insert, items: groups)
                changes.append(change)
            }
        }
        if !updated.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            for (index, group) in self.groups.enumerated() {
                if let _ = updated[group.identifier] {
                    groups.append((index, group))
                }
            }
            if !groups.isEmpty {
                let change = GroupListChange(type: .update, items: groups)
                changes.append(change)
            }
        }
        if !changes.isEmpty {
            let observers = self.observers
            for (_, observer) in observers {
                observer.didChange(self, changes)
            }
        }
    }
    
    
    
}


