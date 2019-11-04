//
//  AssetSelector.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

public final class AssetSelector<ItemType> {
    
//    public typealias ShouldSelect = (_ selector: AssetSelector<ContextType, ResultType>, _ item: Item, _ onResult:@escaping (ResultType?) -> Void) -> Void
//    public typealias ShouldDeselect = (_ selector: AssetSelector<ContextType, ResultType>, _ item: Item, _ onResult:@escaping (Bool) -> Void) -> Void

    
    public struct SelectedItem {
        public let key: String
        public var item: ItemType
    }
    
    public private(set) var items: [SelectedItem] = []

    public let countLimit: UInt
    
    public init(countLimit: UInt) {
        self.countLimit = countLimit
    }
    
    public func selectedItemIndex(forKey key: String) -> Int? {
        for (index, item) in self.items.enumerated() {
            if key == item.key {
                return index
            }
        }
        return nil
    }
    public func replace(_ item: ItemType, forKey key: String) -> Bool {
        if let index = self.selectedItemIndex(forKey: key) {
            var old = self.items[index]
            old.item = item
            self.items[index] = old
            return true
        } else {
            if self.countLimit != 0 && self.items.count >= self.countLimit {
                return false
            }
            let selectedItem = SelectedItem(key: key, item: item)
            self.items.append(selectedItem)
            return true
        }
    }
    public func remove(forKey key: String) -> ItemType? {
        if let index = self.selectedItemIndex(forKey: key) {
            let old = self.items[index]
            self.items.remove(at: index)
            return old.item
        } else {
            return nil
        }
    }
}

