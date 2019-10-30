//
//  AssetSelector.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

public protocol AssetSelectorDelegate: NSObject {
    func selector(_ selector: AssetSelector, shouldSelect: AssetItem, onResult:@escaping (SelectedAssetItem?) -> Void)
    
    //删除成功返回true
    func selector(_ selector: AssetSelector, shouldDeselect: SelectedAssetItem, onResult:@escaping (Bool) -> Void)
}
public final class AssetSelector: NSObject {
    public weak var delegate: AssetSelectorDelegate?
    
    private var itemIdentifierSet: Set<String> = []
    public private(set) var items: [SelectedAssetItem] = []
    
    public init(group: Group) {
        super.init()
    }
    
    private func _append(selectedItem: SelectedAssetItem) {
        self.items.append(selectedItem)
        self.itemIdentifierSet.insert(selectedItem.asset.identifier)
    }
    private func _remove(forIdentifier: String) {
        self.items = self.items.filter { (item) -> Bool in
            return item.asset.identifier != forIdentifier
        }
        self.itemIdentifierSet.remove(forIdentifier)
    }
    
    public func select(item: AssetItem) {
        guard !self.itemIdentifierSet.contains(item.identifier) else {
            return
        }
        guard let delegate = self.delegate else {
            return
        }
        delegate.selector(self, shouldSelect: item) {[weak self] (selectedItem) in
            guard let item = selectedItem else {
                return
            }
            guard let `self` = self else {
                return
            }
            self._append(selectedItem: item)
        }
    }
    public func deselect(item: AssetItem) {
        guard self.itemIdentifierSet.contains(item.identifier) else {
            return
        }
        guard let delegate = self.delegate else {
            return
        }
        
        var selected: SelectedAssetItem? = nil
        for selectedItem in self.items {
            if selectedItem.asset.identifier == item.identifier {
                selected = selectedItem
                break
            }
        }
        guard let selectedItem = selected else {
            return
        }
        delegate.selector(self, shouldDeselect: selectedItem) {[weak self] (result) in
            guard let `self` = self else {
                return
            }
            if result {
                self._remove(forIdentifier: selectedItem.asset.identifier)
            }
        }
    }
}

