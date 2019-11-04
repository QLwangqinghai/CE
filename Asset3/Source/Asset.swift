//
//  Asset.swift
//  Asset
//
//  Created by vector on 2019/11/1.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

public final class AAsset {
//    public let asset: PHAsset
//
//    public var context: ContextType
//    public var identifier: String {
//        return self.asset.localIdentifier
//    }
//    // Playback style describes how the asset should be presented to the user (regardless of the backing media for that asset).  Use this value to choose the type of view and the appropriate APIs on the PHImageManager to display this asset
//    @available(iOS 11, *)
//    open var playbackStyle: PHAsset.PlaybackStyle { get }
//
//    
//    open var mediaType: PHAssetMediaType { get }
//
//    open var mediaSubtypes: PHAssetMediaSubtype { get }
//
//    open var pixelWidth: Int { get }
//    open var pixelHeight: Int { get }
//
//    
//    open var duration: TimeInterval { get }

    
//    @available(iOS 8, *)
//    open func canPerform(_ editOperation: PHAssetEditOperation) -> Bool
//
//
//    @available(iOS 8, *)
//    open class func fetchAssets(in assetCollection: PHAssetCollection, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//    @available(iOS 8, *)
//    open class func fetchAssets(withLocalIdentifiers identifiers: [String], options: PHFetchOptions?) -> PHFetchResult<PHAsset> // includes hidden assets by default
//
//    @available(iOS 8, *)
//    open class func fetchKeyAssets(in assetCollection: PHAssetCollection, options: PHFetchOptions?) -> PHFetchResult<PHAsset>?
//
//    @available(iOS 8, *)
//    open class func fetchAssets(withBurstIdentifier burstIdentifier: String, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//
//    // Fetches PHAssetSourceTypeUserLibrary assets by default (use includeAssetSourceTypes option to override)
//    @available(iOS 8, *)
//    open class func fetchAssets(with options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//    @available(iOS 8, *)
//    open class func fetchAssets(with mediaType: PHAssetMediaType, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//
//    // assetURLs are URLs retrieved from ALAsset's ALAssetPropertyAssetURL
//    @available(iOS, introduced: 8, deprecated: 11, message: "Will be removed in a future release")
//    open class func fetchAssets(withALAssetURLs assetURLs: [URL], options: PHFetchOptions?) -> PHFetchResult<PHAsset>
}


public final class Asset<ContextType> {
    public let asset: PHAsset
    public var context: ContextType
    public var identifier: String {
        return self.asset.localIdentifier
    }
    public init(asset: PHAsset, context: ContextType) {
        self.asset = asset
        self.context = context
    }
}

