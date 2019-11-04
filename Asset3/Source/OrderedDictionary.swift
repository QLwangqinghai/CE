//
//  OrderedDictionary.swift
//  Asset
//
//  Created by vector on 2019/11/1.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


//public enum ArrayOrder {
//    case ascending
//    case descending
//}

//public enum ListChange<Element: Any> {
//    case remove([(Int, Element)])//(oldIndex, (oldOrder, value))
//    case insert([(Int, Element)])//(index, (order, value))
//}


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

