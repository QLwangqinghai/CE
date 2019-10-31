//
//  AssetList.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos


public final class AssetItem<ContextType> {
    public let asset: PHAsset
    public var context: ContextType
    public var identifier: String {
        return self.asset.localIdentifier
    }
    public init<ContextType>(asset: PHAsset, context: ContextType) {
        self.asset = asset
    }
    
}

public final class AssetContext: NSObject {
    public dynamic var isSelected: Bool = false
    public var cachePath: String? = nil
    public init() {

    }
}

public final class AssetList: NSObject {
    public let group: Group
    public let selector: AssetSelector?
    public let context: AssetContext
    private let observerKey: String = UUID().uuidString

    public init(group: Group, context: AssetContext, selector: AssetSelector?) {
        self.group = group
        self.selector = selector
        self.context = context
        super.init()
        
        self.group.addDataObserver({[weak self] (group, changeDetails) in
            guard let `self` = self else {
                return
            }
            guard self.group == group else {
                return
            }
            
        }, forKey: self.observerKey)
    }
    deinit {
        self.group.removeDataObserver(forKey: self.observerKey)
    }
}

