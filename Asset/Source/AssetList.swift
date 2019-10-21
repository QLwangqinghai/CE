//
//  AssetList.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos


public final class AssetContext: NSObject {
    static let subTypeMap: [PHAssetCollectionSubtype: String] = {
         var map:[PHAssetCollectionSubtype: String] = [:]
         map[.albumRegular] = ".albumRegular"
         map[.albumSyncedEvent] = ".albumSyncedEvent"
         map[.albumSyncedFaces] = ".albumSyncedFaces"
         map[.albumSyncedAlbum] = ".albumSyncedAlbum"

         map[.albumImported] = ".albumImported"
         map[.albumMyPhotoStream] = ".albumMyPhotoStream"
         map[.albumCloudShared] = ".albumCloudShared"

         map[.smartAlbumGeneric] = ".smartAlbumGeneric"
         map[.smartAlbumPanoramas] = ".smartAlbumPanoramas"
         map[.smartAlbumVideos] = ".smartAlbumVideos"
         map[.smartAlbumFavorites] = ".smartAlbumFavorites"
         map[.smartAlbumTimelapses] = ".smartAlbumTimelapses"
         map[.smartAlbumAllHidden] = ".smartAlbumAllHidden"
         map[.smartAlbumRecentlyAdded] = ".smartAlbumRecentlyAdded"
         map[.smartAlbumBursts] = ".smartAlbumBursts"
         map[.smartAlbumSlomoVideos] = ".smartAlbumSlomoVideos"
         map[.smartAlbumUserLibrary] = ".smartAlbumUserLibrary"

         map[.smartAlbumSelfPortraits] = ".smartAlbumSelfPortraits"
         map[.smartAlbumScreenshots] = ".smartAlbumScreenshots"
         map[.smartAlbumDepthEffect] = ".smartAlbumDepthEffect"
         map[.smartAlbumLivePhotos] = ".smartAlbumLivePhotos"
         map[.smartAlbumAnimated] = ".smartAlbumAnimated"
         map[.smartAlbumLongExposures] = ".smartAlbumLongExposures"
         map[.smartAlbumUnableToUpload] = ".smartAlbumUnableToUpload"
         return map;
     }()
    
    public private(set) var assetMap: [String: AssetItem] = [:]
    
    subscript(key: String) -> AssetItem? {
        get {
            return self.assetMap[key]
        }
    }
    subscript(asset: PHAsset) -> AssetItem {
        get {
            if let item = self.assetMap[asset.localIdentifier] {
                return item
            } else {
                let item = AssetItem(asset: asset)
                self.assetMap[asset.localIdentifier] = item
                return item
            }
        }
    }
    
    public let cachePath: String
    public init(cachePath: String) {
        self.cachePath = cachePath
    }
}


public final class SelectedAssetItem: NSObject {
    public let asset: AssetItem
    public var info: [String: Any] = [:]
    public init(asset: AssetItem) {
        self.asset = asset
    }
    
}
public final class AssetItem: NSObject {
    public let asset: PHAsset
    public var identifier: String {
        return self.asset.localIdentifier
    }
    public init(asset: PHAsset) {
        self.asset = asset
    }
    
}

public final class AssetList: NSObject {
    public let group: AssetGroup
    public let selector: AssetSelector?
    public let context: AssetContext
    
    public init(group: AssetGroup, context: AssetContext, selector: AssetSelector?) {
        self.group = group
        self.selector = selector
        self.context = context
        super.init()
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
}

