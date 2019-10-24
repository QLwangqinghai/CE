//
//  OrderedArray.swift
//  TTable
//
//  Created by vector on 2019/10/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public enum ListOrder {
    case ascending
    case escending
}

public struct OrderedArray<Order, Value> where Order: Comparable {
    public private(set) var array: [(Order, Value)] = []
    public private(set) var order: ListOrder
    public init(order: ListOrder = .ascending) {
        self.order = order
    }
    
    public func goodInsertIndex(order: Order) -> Int {
        guard !self.array.isEmpty else {
            return 0
        }
        for (index, item) in self.array.enumerated() {
            if self._compare(lhs: order, rhs: item.0) {
                return index
            }
        }
        return array.count
    }
    private func _compare(lhs: Order, rhs: Order) -> Bool {
        switch self.order {
        case .ascending:
            return lhs < rhs
        case .escending:
            return lhs > rhs
        }
    }
    
    @discardableResult public mutating func filter(_ body: (Order, Value) -> Bool) -> [(Int, Order, Value)] {
        var array: [(Order, Value)] = []
        var removed: [(Int, Order, Value)] = []
        for (index, item) in self.array.enumerated() {
            if body(item.0, item.1) {
                array.append(item)
            } else {
                removed.append((index, item.0, item.1))
            }
        }
        self.array = array
        return removed
    }
    
    @discardableResult public mutating func removeItems<S>(in indexs: S) -> [(Int, Order, Value)] where Int == S.Element, S : Sequence  {
        var idxs: Set<Int> = []
        for idx in indexs {
            idxs.insert(idx)
        }
        return self.removeItems(at: idxs)
    }
    
    @discardableResult public mutating func removeItems(at indexs: Set<Int>) -> [(Int, Order, Value)] {
        var array: [(Order, Value)] = []
        var removed: [(Int, Order, Value)] = []
        for (index, item) in self.array.enumerated() {
            if indexs.contains(index) {
                removed.append((index, item.0, item.1))
            } else {
                array.append(item)
            }
        }
        self.array = array
        return removed
    }
    
    @discardableResult public mutating func insertItems<S>(_ items: S) -> [(Int, Order, Value)] where (Order, Value) == S.Element, S : Sequence {
        var array: [(Order, Value)] = []
        var inserted: [(Int, Order, Value)] = []
        var ordered = items.sorted { (lhs, rhs) -> Bool in
            return self._compare(lhs: lhs.0, rhs: rhs.0)
        }
        
        for item in self.array {
            while let insertItem = ordered.first {
                if self._compare(lhs: insertItem.0, rhs: item.0) {
                    //顺序不可调整
                    inserted.append((array.count, insertItem.0, insertItem.1))
                    array.append(insertItem)
                    ordered.removeFirst()
                } else {
                    break
                }
            }
            array.append(item)
        }
        while let insertItem = ordered.first {
            //顺序不可调整
            inserted.append((array.count, insertItem.0, insertItem.1))
            array.append(insertItem)
            ordered.removeFirst()
        }
        self.array = array
        return inserted
    }
    
    public mutating func replaceValues(_ body: (Order, Value) -> Value) {
        for (index, item) in self.array.enumerated() {
            self.array[index] = (item.0, body(item.0, item.1))
        }
    }
}
