//
//  AssetList.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos



public final class SelectedItem {
//    public var thumbnailIdentifier: ThumbnailIdentifier
//    public var path: String
//    public var mediaSubtype: PHAssetMediaSubtype
    public init() {

    }
}

public final class AssetList: NSObject {
    public typealias Item = AssetItem<AssetContext>
    public typealias Group = AssetGroup<ProviderContext, CachingImageManager, AssetContext>
    public typealias Selector = AssetSelector<(AssetContext)>

    public let group: Group
    public let selector: Selector?
    public let builder: ProviderContext
    private let observerKey: String = UUID().uuidString

    public private(set) var items: [Item] = []
    
    
    public init(group: Group, builder: ProviderContext, selector: Selector?) {
        self.group = group
        self.builder = builder
        self.selector = selector
        super.init()
        
        self.group.addDataObserver({[weak self] (group, changeDetails) in
            guard let `self` = self else {
                return
            }
            guard self.group === group else {
                return
            }
            
        }, forKey: self.observerKey)
    }
    deinit {
        self.group.removeDataObserver(forKey: self.observerKey)
    }
}

