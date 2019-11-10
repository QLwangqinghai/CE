//
//  Cache.swift
//  Asset
//
//  Created by vector on 2019/10/31.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

fileprivate class WeakContainer<ObjectType : AnyObject> {
    private weak var storage: ObjectType?
    var value: ObjectType? {
        if let v = self.storage {
            return v
        } else {
            self.storage = nil
            return nil
        }
    }
    init(value: ObjectType) {
        self.storage = value
    }
}
fileprivate class CacheEntry<KeyType : AnyObject, ObjectType : AnyObject> {
    var key: KeyType
    var value: ObjectType
    var cost: Int
    var prevByCost: CacheEntry?
    var nextByCost: CacheEntry?
    init(key: KeyType, value: ObjectType, cost: Int) {
        self.key = key
        self.value = value
        self.cost = cost
    }
}

open class MemoryCache<KeyType : AnyObject & Hashable, ObjectType : AnyObject> {
    private var weakDictionary = Dictionary<KeyType, WeakContainer<ObjectType>>()
    private var _entries = Dictionary<KeyType, CacheEntry<KeyType, ObjectType>>()
    private let _lock = NSLock()
    private var _totalCost = 0
    private var _head: CacheEntry<KeyType, ObjectType>?
    
    open var name: String = ""
    open var totalCostLimit: Int = 0 // limits are imprecise/not strict

    public init() {}
    
    open func object(forKey key: KeyType) -> ObjectType? {
        var object: ObjectType?
        
        _lock.lock()
        if let entry = _entries[key] {
            object = entry.value
        } else {
            if let entry = self.weakDictionary[key] {
                object = entry.value
                if nil == object {
                    self.weakDictionary.removeValue(forKey: key)
                }
            }
        }
        _lock.unlock()
        
        return object
    }
    
    private func remove(_ entry: CacheEntry<KeyType, ObjectType>) {
        let oldPrev = entry.prevByCost
        let oldNext = entry.nextByCost
        
        oldPrev?.nextByCost = oldNext
        oldNext?.prevByCost = oldPrev
        
        if entry === _head {
            _head = oldNext
        }
    }
   
    private func insert(_ entry: CacheEntry<KeyType, ObjectType>) {
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
    
    open func setObject(_ obj: ObjectType, forKey key: KeyType, cost g: Int) {
        let g = max(g, 0)
        let keyRef = key
        
        _lock.lock()
        
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
            let entry = CacheEntry(key: key, value: obj, cost: g)
            _entries[keyRef] = entry
            insert(entry)
            
            costDiff = g
        }
        self.weakDictionary[key] = WeakContainer<ObjectType>(value: obj)
        
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
                if let entry = self.weakDictionary[key] {
                    if nil == entry.value {
                        self.weakDictionary.removeValue(forKey: key)
                    }
                }
            } else {
                break
            }
        }
        _lock.unlock()
    }
    
    open func removeObjectStrongRefrence(forKey key: KeyType) {
        let keyRef = key
        
        _lock.lock()
        
        autoreleasepool { () -> Void in
            if let entry = _entries.removeValue(forKey: keyRef) {
                _totalCost -= entry.cost
                remove(entry)
            }
        }
        if let entry = self.weakDictionary[key] {
            if nil == entry.value {
                self.weakDictionary.removeValue(forKey: key)
            }
        }
        _lock.unlock()
    }
    
    open func removeAllObjectStrongRefrences() {
        _lock.lock()
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
        
        self.weakDictionary = self.weakDictionary.compactMapValues { (entry) -> WeakContainer<ObjectType>? in
            if nil == entry.value {
                return nil
            } else {
                return entry
            }
        }
        
        _lock.unlock()
    }
}
