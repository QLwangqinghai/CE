//
//  AssetSelector.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

public final class AssetSelector<ContextType, ResultType> {
    public typealias Item = AssetItem<ContextType>
    
    public typealias ShouldSelect = (_ selector: AssetSelector<ContextType, ResultType>, _ item: Item, _ onResult:@escaping (ResultType?) -> Void) -> Void
    public typealias ShouldDeselect = (_ selector: AssetSelector<ContextType, ResultType>, _ item: Item, _ onResult:@escaping (Bool) -> Void) -> Void

    
    public struct SelectedItem {
        public let sequence: Int
        public let asset: Item
        public let result: ResultType
    }
    
    private var sequence: Int = 1
    public private(set) var items: [String: SelectedItem] = [:]
    
    public var shouldSelect: ShouldSelect
    public var shouldDeselect: ShouldDeselect

    public init(group: Group, shouldSelect: @escaping ShouldSelect, shouldDeselect: @escaping ShouldDeselect) {
        self.shouldSelect = shouldSelect
        self.shouldDeselect = shouldDeselect
    }
    
    private func _append(_ asset: Item, result: ResultType, forKey key: String) {
        self.sequence += 1
        let item = SelectedItem(sequence: self.sequence, asset: asset, result: result)
        self.items[key] = item
    }
    private func _remove(forKey key: String) {
        self.items[key] = nil
    }
    
    public func select(asset: Item) {
        guard nil == self.items[asset.identifier] else {
            return
        }
        let key = asset.identifier
        self.shouldSelect(self, asset, {[weak self] (result) -> Void in
            guard let `self` = self else {
                return
            }
            guard let value = result else {
                return
            }
            self._append(asset, result: value, forKey: key)
        })
    }
    public func deselect(forKey key: String) {
        guard let item = self.items[key] else {
            return
        }
        self.shouldDeselect(self, item.asset, {[weak self] (result) -> Void in
            guard let `self` = self else {
                return
            }
            if result {
                self._remove(forKey: key)
            }
        })
    }
}

