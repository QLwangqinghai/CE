//
//  FileWritingQueue.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/22.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit



public protocol DiscardableObject: class {
    func beginContentAccess() -> Bool
    func endContentAccess()
    func discardContentIfPossible()
    func isContentDiscarded() -> Bool
}

@propertyWrapper public class Discardable<Value> where Value: (DiscardableObject & Equatable) {

    public init(wrappedValue: Value) {
        self._value = wrappedValue
    }
    private var _value: Value

    public var wrappedValue: Value {
        get { self._value }
        set {
            self._value = newValue
            
//            $.
//            publisher?.subject.value = newValue
        }
    }

}


public final class FileWritingQueue {
    fileprivate class WritingEntry {
        var key: String
        var value: FileData
        var cost: Int
        var prevByCost: WritingEntry?
        var nextByCost: WritingEntry?
        init(key: String, value: FileData, cost: Int) {
            self.key = key
            self.value = value
            self.cost = cost
        }
    }
    
    private var _entries = Dictionary<String, WritingEntry>()
    private var _totalCost = 0
    private var _head: WritingEntry?
    
    public var name: String = ""
    public var totalCostLimit: Int = 0 // limits are imprecise/not strict

    public init() {}
    
    public func object(forKey key: String) -> FileData? {
        var object: FileData?
        if let entry = _entries[key] {
            object = entry.value
        }
        return object
    }
    
    public func setObject(_ obj: FileData, forKey key: String) {
        self.setObject(obj, forKey: key, cost: 0)
    }
    
    private func remove(_ entry: WritingEntry) {
        let oldPrev = entry.prevByCost
        let oldNext = entry.nextByCost
        
        oldPrev?.nextByCost = oldNext
        oldNext?.prevByCost = oldPrev
        
        if entry === _head {
            _head = oldNext
        }
    }
   
    private func insert(_ entry: WritingEntry) {
        guard var currentElement = _head else {
            // The cache is empty
            entry.prevByCost = nil
            entry.nextByCost = nil
            
            _head = entry
            return
        }
        
        guard entry.cost > currentElement.cost else {
            // Insert entry at the head
            entry.prevByCost = nil
            entry.nextByCost = currentElement
            currentElement.prevByCost = entry
            
            _head = entry
            return
        }
        
        while let nextByCost = currentElement.nextByCost, nextByCost.cost < entry.cost {
            currentElement = nextByCost
        }
        
        // Insert entry between currentElement and nextElement
        let nextElement = currentElement.nextByCost
        
        currentElement.nextByCost = entry
        entry.prevByCost = currentElement
        
        entry.nextByCost = nextElement
        nextElement?.prevByCost = entry
    }
    
    public func setObject(_ obj: FileData, forKey key: String, cost g: Int) {
        let g = max(g, 0)
        let keyRef = key
                
        let costDiff: Int
        
        if let entry = _entries[keyRef] {
            costDiff = g - entry.cost
            entry.cost = g
            
            entry.value = obj
            
            if costDiff != 0 {
                remove(entry)
                insert(entry)
            }
        } else {
            let entry = WritingEntry(key: key, value: obj, cost: g)
            _entries[keyRef] = entry
            insert(entry)
            
            costDiff = g
        }
        
        _totalCost += costDiff
        
        var purgeAmount = (totalCostLimit > 0) ? (_totalCost - totalCostLimit) : 0
                
        while purgeAmount > 0 {
            if let entry = _head {
                _totalCost -= entry.cost
                purgeAmount -= entry.cost
                
                let key = entry.key
                
                autoreleasepool { () -> Void in
                    remove(entry) // _head will be changed to next entry in remove(_:)
                    _entries[key] = nil
                }
            } else {
                break
            }
        }
    }
    
    public func removeObjectStrongRefrence(forKey key: String) {
        let keyRef = key
        autoreleasepool { () -> Void in
            if let entry = _entries.removeValue(forKey: keyRef) {
                _totalCost -= entry.cost
                remove(entry)
            }
        }
    }
    
    public func removeAllObjectStrongRefrences() {
        _entries.removeAll()
        autoreleasepool { () -> Void in
            while let currentElement = _head {
                let nextElement = currentElement.nextByCost
                
                currentElement.prevByCost = nil
                currentElement.nextByCost = nil
                
                _head = nextElement
            }
        }
        _totalCost = 0
    }
}
