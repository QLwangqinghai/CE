//
//  Asset.swift
//  Clould
//
//  Created by vector on 2019/11/4.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

public final class Asset {
    public enum PlaybackStyle : Int {
        case unsupported = 0x0
        case image = 0x1
        case imageAnimated = 0x2
        case livePhoto = 0x3
        case video = 0x8000
        case videoLooping = 0x8001
    }
    
    public struct Identifier {
        let uuid: uuid_t

    }
    
    
    public let playbackStyle: PlaybackStyle = .image
    public let identifier: String = ""
    public var creationDate: Int64 = 0
    public var modificationDate: Int64 = 0
    public var duration: Int64 = 0
    public var mask: UInt64 = 0
    public var burstIdentifier: String? = nil
    
    public init() {
        
    }
    
}


//// Playback style describes how the asset should be presented to the user (regardless of the backing media for that asset).  Use this value to choose the type of view and the appropriate APIs on the PHImageManager to display this asset
//@available(iOS 11, *)
//open var playbackStyle: PHAsset.PlaybackStyle { get }
//
//
//@available(iOS 8, *)
//open var mediaType: PHAssetMediaType { get }
//
//@available(iOS 8, *)
//open var mediaSubtypes: PHAssetMediaSubtype { get }
//
//
//@available(iOS 8, *)
//open var pixelWidth: Int { get }
//
//@available(iOS 8, *)
//open var pixelHeight: Int { get }
//
//
//@available(iOS 8, *)
//open var creationDate: Date? { get }
//
//@available(iOS 8, *)
//open var modificationDate: Date? { get }
//
//
//@available(iOS 8, *)
//open var location: CLLocation? { get }
//
//
//@available(iOS 8, *)
//open var duration: TimeInterval { get }
//
//
//// a hidden asset will be excluded from moment collections, but may still be included in other smart or regular album collections
//@available(iOS 8, *)
//open var isHidden: Bool { get }
//
//
//@available(iOS 8, *)
//open var isFavorite: Bool { get }
//
//
//@available(iOS 8, *)
//open var burstIdentifier: String? { get }
//
//@available(iOS 8, *)
//open var burstSelectionTypes: PHAssetBurstSelectionType { get }
//
//@available(iOS 8, *)
//open var representsBurst: Bool { get }
//
//
//@available(iOS 9, *)
//open var sourceType: PHAssetSourceType { get }
//
//
//@available(iOS 8, *)
//open func canPerform(_ editOperation: PHAssetEditOperation) -> Bool
//
//
//@available(iOS 8, *)
//open class func fetchAssets(in assetCollection: PHAssetCollection, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//@available(iOS 8, *)
//open class func fetchAssets(withLocalIdentifiers identifiers: [String], options: PHFetchOptions?) -> PHFetchResult<PHAsset> // includes hidden assets by default
//
//@available(iOS 8, *)
//open class func fetchKeyAssets(in assetCollection: PHAssetCollection, options: PHFetchOptions?) -> PHFetchResult<PHAsset>?
//
//@available(iOS 8, *)
//open class func fetchAssets(withBurstIdentifier burstIdentifier: String, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//
//// Fetches PHAssetSourceTypeUserLibrary assets by default (use includeAssetSourceTypes option to override)
//@available(iOS 8, *)
//open class func fetchAssets(with options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//@available(iOS 8, *)
//open class func fetchAssets(with mediaType: PHAssetMediaType, options: PHFetchOptions?) -> PHFetchResult<PHAsset>
//
//
//// assetURLs are URLs retrieved from ALAsset's ALAssetPropertyAssetURL
//@available(iOS, introduced: 8, deprecated: 11, message: "Will be removed in a future release")
//open class func fetchAssets(withALAssetURLs assetURLs: [URL], options: PHFetchOptions?) -> PHFetchResult<PHAsset>
