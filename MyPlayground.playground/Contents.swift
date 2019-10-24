import UIKit

public enum ListChange<Element: Any> {
    case remove([(Int, Element)])
    case insert([(Int, Element)])
}

public protocol UniqueElement {
    associatedtype UniqueIdentifier: Hashable
    var uniqueIdentifier: UniqueIdentifier {
        get
    }

    func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String)
    func unobserveContent(forKey:String)
}

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

public struct UniqueOrderedListUpdater<Order, Value> where Value: UniqueElement, Order: Comparable {
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

public class UniqueOrderedList<Order, Value> where Value: UniqueElement, Order: Comparable {
    public typealias Key = Value.UniqueIdentifier

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
        
        var remain = dictionary
        for (key, item) in oldDictionary {
            if let newItem = dictionary[key] {
                if newItem.0 != item.0 {
                    waitRemoved.insert(key)
                } else {
                    remain.removeValue(forKey: key)
                }
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
    
    public func update(_ body: (_ updater: inout UniqueOrderedListUpdater<Order, Value>) -> Void) {
        var updater = UniqueOrderedListUpdater<Order, Value>(dictionary: self.dictionary)
        body(&updater)
        self._update(dictionary: updater.dictionary)
    }
}





let label = UILabel()

print(label.intrinsicContentSize)
label.text = ""
print(label.intrinsicContentSize)



