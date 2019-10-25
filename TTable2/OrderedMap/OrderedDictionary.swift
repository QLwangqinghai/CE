//
//  OrderedDictionary.swift
//  OrderedMap
//
//  Created by vector on 2019/10/25.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum ListOrder {
    case ascending
    case escending
}
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

public class UniqueOrderedList<Order, Value> where Order: Comparable, Value: UniqueElement {
    public typealias Key = Value.UniqueIdentifier
    
    
    fileprivate class _Item {
        let key: Key
        var index: Int = NSNotFound
        var order: Order
        var value: Value
        init(key: Key, order: Order, value: Value) {
            self.order = order
            self.key = key
            self.value = value
        }
    }

    public struct Updater {
        fileprivate var dictionary: [Key: _Item]
        fileprivate init(dictionary: [Key: _Item]) {
            self.dictionary = dictionary
        }
        public mutating func filter(_ body: (Order, Value) -> Bool) {
            self.dictionary = self.dictionary.filter({ (item) -> Bool in
                return body(item.value.order, item.value.value)
            })
        }
        public mutating func replace<S>(_ items: S) where (Order, Value) == S.Element, S : Sequence {
            for item in items {
                let key = item.1.uniqueIdentifier
                if let object = self.dictionary[key] {
                    object.order = item.0
                    object.value = item.1
                } else {
                    let object: _Item = _Item(key: key, order: item.0, value: item.1)
                    self.dictionary[key] = object
                }
            }
        }
        public mutating func removeAll() {
            self.dictionary.removeAll()
        }
    }

    
    public typealias Change = ListChange<(Order, Value)>
    public typealias ObserverClosure = (_ entity: UniqueOrderedList<Order, Value>, _ changes: [Change]) -> Void

    private var array: [(Order, _Item)]
    private var dictionary: [Key: _Item]

    private let contentObserveKey: String = UUID().uuidString
    private var observers: [AnyHashable: ObserverClosure] = [:]
    public private(set) var order: ListOrder

    private func _compare(lhs: Order, rhs: Order) -> Bool {
        switch self.order {
        case .ascending:
            return lhs < rhs
        case .escending:
            return lhs > rhs
        }
    }

    public init(order: ListOrder = .ascending) {
        self.order = order
        self.array = []
        self.dictionary = [:]
    }
    
    public func load() -> [Value] {
//        return self.orderedArray.array.map { (item) -> Value in
//            return item.1
//        }
        return []
    }
    
    public func observeList(didChange: @escaping ObserverClosure, forKey: AnyHashable) {
        self.observers[forKey] = didChange
    }
    public func unobserveList(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    private func _update(dictionary: [Key: _Item]) {
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
    
    public func update(_ body: (_ updater: inout Updater) -> Void) {
        var updater = Updater(dictionary: self.dictionary)
        body(&updater)
//        self._update(dictionary: updater.dictionary)
    }





public struct OrderedArray {
    fileprivate var array: [_Item] = []
    public private(set) var order: ListOrder
    public init(order: ListOrder = .ascending) {
        self.order = order
    }
    
//    public func goodInsertIndex(order: Order) -> Int {
//        var r: Range<Int> = Range<Int>.init(uncheckedBounds: <#T##(lower: Int, upper: Int)#>)
//        guard !self.array.isEmpty else {
//            return 0
//        }
//        for (index, item) in self.array.enumerated() {
//            if self._compare(lhs: order, rhs: item.0) {
//                return index
//            }
//        }
//        return array.count
//    }
//    public func goodInsertIndex(order: Order, range: Range<Int>) -> Int {
//        var r: Range
//        guard !self.array.isEmpty else {
//            return 0
//        }
//        for (index, item) in self.array.enumerated() {
//            if self._compare(lhs: order, rhs: item.0) {
//                return index
//            }
//        }
//        return array.count
//    }
    private func _compare(lhs: Order, rhs: Order) -> Bool {
        switch self.order {
        case .ascending:
            return lhs < rhs
        case .escending:
            return lhs > rhs
        }
    }
    
//    @discardableResult public mutating func filter(_ body: (Order, Value) -> Bool) -> [(Int, Order, Value)] {
//        var array: [(Order, Value)] = []
//        var removed: [(Int, Order, Value)] = []
//        for (index, item) in self.array.enumerated() {
//            if body(item.0, item.1) {
//                array.append(item)
//            } else {
//                removed.append((index, item.0, item.1))
//            }
//        }
//        self.array = array
//        return removed
//    }
//
//    @discardableResult public mutating func removeItems<S>(in indexs: S) -> [(Int, Order, Value)] where Int == S.Element, S : Sequence  {
//        var idxs: Set<Int> = []
//        for idx in indexs {
//            idxs.insert(idx)
//        }
//        return self.removeItems(at: idxs)
//    }
//
//    @discardableResult public mutating func removeItems(at indexs: Set<Int>) -> [(Int, Order, Value)] {
//        var array: [(Order, Value)] = []
//        var removed: [(Int, Order, Value)] = []
//        for (index, item) in self.array.enumerated() {
//            if indexs.contains(index) {
//                removed.append((index, item.0, item.1))
//            } else {
//                array.append(item)
//            }
//        }
//        self.array = array
//        return removed
//    }
//
//    @discardableResult public mutating func insertItems<S>(_ items: S) -> [(Int, Order, Value)] where (Order, Value) == S.Element, S : Sequence {
//        var array: [(Order, Value)] = []
//        var inserted: [(Int, Order, Value)] = []
//        var ordered = items.sorted { (lhs, rhs) -> Bool in
//            return self._compare(lhs: lhs.0, rhs: rhs.0)
//        }
//
//        for item in self.array {
//            while let insertItem = ordered.first {
//                if self._compare(lhs: insertItem.0, rhs: item.0) {
//                    //顺序不可调整
//                    inserted.append((array.count, insertItem.0, insertItem.1))
//                    array.append(insertItem)
//                    ordered.removeFirst()
//                } else {
//                    break
//                }
//            }
//            array.append(item)
//        }
//        while let insertItem = ordered.first {
//            //顺序不可调整
//            inserted.append((array.count, insertItem.0, insertItem.1))
//            array.append(insertItem)
//            ordered.removeFirst()
//        }
//        self.array = array
//        return inserted
//    }
//
//    public mutating func replaceValues(_ body: (Order, Value) -> Value) {
//        for (index, item) in self.array.enumerated() {
//            self.array[index] = (item.0, body(item.0, item.1))
//        }
//    }
//
    
    public struct OrderChange {
        public let oldOrder: Order
        public let newOrder: Order
        public let value: Value
        public init(oldOrder: Order, newOrder: Order, value: Value) {
            self.oldOrder = oldOrder
            self.newOrder = newOrder
            self.value = value
        }
    }
    private struct _Change {
        var changed: Bool {
            return self.change.oldOrder != self.change.newOrder
        }
        let change: OrderChange
        var oldIndex: Int
        var newIndex: Int
        init(change: OrderChange, oldIndex: Int, newIndex: Int) {
            self.change = change
            self.oldIndex = oldIndex
            self.newIndex = newIndex
        }
    }
    
//    public func binarySearch(order: Order) -> Int? {
//        var start = 0
//        var end = self.array.count - 1
//        while start <= end {
//            let minddle = (start + end)/2
//            let mOrder = self.array[minddle].0
//            if order == mOrder {
//                return minddle
//            } else {
//                if self._compare(lhs: order, rhs: mOrder) {
//                    end = minddle - 1
//                } else {
//                    start = minddle + 1
//                }
//            }
//        }
//        return nil
//    }
    
//    @discardableResult public mutating func updateOrder(at index: Int, newOrder: Order) -> Int? {
//        func goodInsertIndex(order: Order, start: Int, end: Int) -> Int {
//            if end - start <= 0 {
//                return end
//            }
//            for index in start ... end {
//                if self._compare(lhs: order, rhs: self.array[index].0) {
//                    return index
//                }
//            }
//            return end
//        }
//
//        let old = self.array[index]
//        guard old.0 != newOrder else {
//            return nil
//        }
//        self.array[index] = (newOrder, old.1)
//        guard self.array.count > 1 else {
//            return nil
//        }
//
//        if index == self.array.startIndex {
//            if self._compare(lhs: self.array[index].0, rhs: self.array[index + 1].0) {
//                return nil
//            }
//        } else if index == self.array.endIndex {
//            if self._compare(lhs: self.array[index-1].0, rhs: self.array[index].0) {
//                return nil
//            }
//        } else {
//            if self._compare(lhs: self.array[index-1].0, rhs: self.array[index].0) && self._compare(lhs: self.array[index].0, rhs: self.array[index + 1].0) {
//                return nil
//            }
//        }
//
//        self.array.remove(at: index)
//        let insertIndex: Int
//        if self._compare(lhs: old.0, rhs: newOrder) {//right side insert
//            insertIndex = goodInsertIndex(order: newOrder, start: index, end: self.array.count - 1)
//        } else {//left side insert
//            insertIndex = goodInsertIndex(order: newOrder, start: 0, end: index - 1)
//        }
//        self.array.insert((newOrder, old.1), at: insertIndex)
//        return insertIndex
//    }
    
//        @discardableResult public mutating func updateOrder(_ body: (Order, Value) -> Order) -> [(Int/*old*/, Int/*to*/, OrderChange)] {
//            let oldMap = self.array
//            var result: [(Int/*old*/, Int/*to*/, OrderChange)] = []
//            var array: [_Change] = []
//            var tmpList: [Int] = []
//            var newIndexs: [(Order, Int)] = []
//
//            var changeMap: [Int: OrderChange] = [:]
//            for (k, item) in oldMap.enumerated() {
//                let change = OrderChange(oldOrder:item.0, newOrder: body(item.0, item.1), value: item.1)
//                tmpList.append(k)
//                newIndexs.append((change.newOrder, k))
//                if change.oldOrder != change.newOrder {
//                    changeMap[k] = change
//                }
//            }
//
//
//
//    //        var sorted: [(Order, Int)] = newIndexs.sorted { (lhs, rhs) -> Bool in
//    //            return self._compare(lhs: lhs.0, rhs: rhs.0)
//    //        }
//    //
//    //        for (idx, item) in sorted.enumerated() {
//    //            let k = tmpList[idx]
//    //            if k != item.1 {
//    //
//    //            }
//    //        }
//
//
//
//            for (index, item) in sorted.enumerated() {
//                var tmp = item
//                tmp.newIndex = index
//                sorted[index] = tmp
//            }
//            for (index, item) in sorted.enumerated() {
//                if item.1.old != item.0 {
//                    if index != item.oldIndex {
//                        result.append((item.oldIndex, index, item.value))
//                    }
//                }
//            }
//            return []
//        }

}


}
