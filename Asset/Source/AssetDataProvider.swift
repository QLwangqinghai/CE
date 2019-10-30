//
//  AssetGroupDataSource.swift
//  Photo
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos
import UIKit

/*
 PHAssetCollectionType
 
 album //从 iTunes 同步来的相册，以及用户在 Photos 中自己建立的相册
 smartAlbum //经由相机得来的相册
 moment //废弃的，为我们自动生成的时间分组的相册
 */

/*
 PHAssetCollectionSubtype
 albumRegular //用户在 Photos 中创建的相册，也就是我所谓的逻辑相册
 albumSyncedEvent //使用 iTunes 从 Photos 照片库或者 iPhoto 照片库同步过来的事件。
 albumSyncedFaces //使用 iTunes 从 Photos 照片库或者 iPhoto 照片库同步的人物相册。
 albumSyncedAlbum //做了 AlbumSyncedEvent 应该做的事
 albumImported //从相机或是外部存储导入的相册。
 albumMyPhotoStream //用户的 iCloud 照片流
 albumCloudShared //用户使用 iCloud 共享的相册
 
 smartAlbumGeneric //文档解释为非特殊类型的相册，主要包括从 iPhoto 同步过来的相册。由于本人的 iPhoto 已被 Photos 替代，无法验证。不过，在我的 iPad mini 上是无法获取的，而下面类型的相册，尽管没有包含照片或视频，但能够获取到。
 smartAlbumPanoramas //全景照片 相机拍摄的全景照片
 smartAlbumVideos //视频 相机拍摄的视频
 smartAlbumFavorites //个人收藏 收藏文件夹
 smartAlbumTimelapses //延时摄影 延时视频文件夹，同时也会出现在视频文件夹中
 smartAlbumAllHidden //已隐藏 包含隐藏照片或视频的文件夹
 smartAlbumRecentlyAdded //  最近项目 相机近期拍摄的照片或视频
 smartAlbumBursts //连拍快照 连拍模式拍摄的照片
 smartAlbumSlomoVideos //慢动作 Slomo 是 slow motion 的缩写，高速摄影慢动作解析，在该模式下，iOS 设备以120帧拍摄。
 smartAlbumUserLibrary //最近项目 相机相册，所有相机拍摄的照片或视频都会出现在该相册中，而且使用其他应用保存的照片也会出现在这里。
 any //全部类型

     @available(iOS 9, *)
     case smartAlbumSelfPortraits //自拍 视频 + 图片

     @available(iOS 9, *)
     case smartAlbumScreenshots //截屏

     @available(iOS 10.2, *)
     case smartAlbumDepthEffect //人像

     @available(iOS 10.3, *)
     case smartAlbumLivePhotos //实况照片

     @available(iOS 11, *)
     case smartAlbumAnimated //动图
 
     @available(iOS 11, *)
     case smartAlbumLongExposures //长曝光
 
     @available(iOS 13, *)
     case smartAlbumUnableToUpload
//图库： 照片 回忆 个人收藏 任务 地点 导入
 //媒体类型： 视频 自拍 实况照片 延时摄影 连拍快照 屏幕快照 动图
 }
 */


/*
 public enum PHAssetMediaType : Int {

     
     @available(iOS 8, *)
     case unknown

     @available(iOS 8, *)
     case image

     @available(iOS 8, *)
     case video

     @available(iOS 8, *)
     case audio
 }

 @available(iOS 8, iOS 8, *)
 public struct PHAssetMediaSubtype : OptionSet {

     public init(rawValue: UInt)

     
     // Photo subtypes
     @available(iOS 8, *)
     public static var photoPanorama: PHAssetMediaSubtype { get }

     @available(iOS 8, *)
     public static var photoHDR: PHAssetMediaSubtype { get }

     @available(iOS 9, *)
     public static var photoScreenshot: PHAssetMediaSubtype { get }

     @available(iOS 9.1, *)
     public static var photoLive: PHAssetMediaSubtype { get }

     @available(iOS 10.2, *)
     public static var photoDepthEffect: PHAssetMediaSubtype { get }

     
     // Video subtypes
     @available(iOS 8, *)
     public static var videoStreamed: PHAssetMediaSubtype { get }

     @available(iOS 8, *)
     public static var videoHighFrameRate: PHAssetMediaSubtype { get }

     @available(iOS 8, *)
     public static var videoTimelapse: PHAssetMediaSubtype { get }
 }

 */


open class AssetDataProvider: NSObject {
    public final class Options {
        //Thumbnail
        public let identifier: String
        public let mode: Mode

        fileprivate init(mode: Mode) {
            self.mode = mode
            self.identifier = UUID().uuidString
        }
        
//        lazy var cachingImageManager: PHCachingImageManager = {
//            return PHCachingImageManager()
//        }()
//        @discardableResult public func requestThumbnailImage(asset: PHAsset, size: CGSize, resultHandler: @escaping (_ requestId: PHImageRequestID, _ image: UIImage?, _ error: NSError?) -> Void) -> PHImageRequestID? {
//            let options = PHImageRequestOptions()
//            options.isNetworkAccessAllowed = true
//            options.resizeMode = .exact
//            options.isSynchronous = true
//            let id = self.cachingImageManager.requestImage(for: asset, targetSize: size, contentMode: .aspectFill, options: options) { (image, info) in
//                DispatchQueue.main.async {
//                    var rid = PHInvalidImageRequestID
//                    if let number = info?[PHImageResultRequestIDKey] as? NSNumber {
//                        rid = PHImageRequestID(truncating: number)
//                    }
//
//                    if let error = info?[PHImageErrorKey] as? NSError {
//                        resultHandler(rid, image, error)
//                    } else {
//                        resultHandler(rid, image, nil)
//                    }
//                }
//            }
//            if id == PHInvalidImageRequestID {
//                return nil
//            } else {
//                return id
//            }
//        }
    }
    
    public enum Mode {
        case image
        case video
        case imageAndVideo
    }
    
    public var mode: Mode {
        return self.option.mode
    }
    public var identifier: String {
        return self.option.identifier
    }
    public private(set) var smartAlbumFetchResult: PHFetchResult<PHAssetCollection>
    public private(set) var albumFetchResult: PHFetchResult<PHAssetCollection>
    
    public private(set) var groupArray: [AssetGroup] = []
    public private(set) var groupDictionary: [String: AssetGroup] = [:]
    
    public let option: Options
    
    public init(mode: Mode = .imageAndVideo) {
        self.option = Options(mode: mode)
        self.smartAlbumFetchResult = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        self.albumFetchResult = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        super.init()
        PHPhotoLibrary.shared().register(self)
        self._loadAllGroups()
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
    
    private static let _videoOnlyAssetCollectionSubtypes: Set<PHAssetCollectionSubtype> = {
        var types: Set<PHAssetCollectionSubtype> = [.smartAlbumTimelapses, .smartAlbumSlomoVideos, .smartAlbumVideos]
        return types
    }()
    
    private static let _imageOnlyAssetCollectionSubtypes: Set<PHAssetCollectionSubtype> = {
        var types: Set<PHAssetCollectionSubtype> = [.smartAlbumPanoramas, .smartAlbumBursts]
        if #available(iOS 9, *) {
            types.insert(.smartAlbumScreenshots)
        }
        if #available(iOS 10.2, *) {
            types.insert(.smartAlbumDepthEffect)
        }
        if #available(iOS 10.3, *) {
            types.insert(.smartAlbumLivePhotos)
        }
        if #available(iOS 11, *) {
            types.insert(.smartAlbumAnimated)
            types.insert(.smartAlbumLongExposures)
        }
        return types
    }()
    
    private func _allowAssetCollectionSubtype(_ subtype: PHAssetCollectionSubtype) -> Bool {
        if subtype == .smartAlbumAllHidden {
            return false
        }
        if self.mode == .image {
            return !AssetDataProvider._videoOnlyAssetCollectionSubtypes.contains(subtype)
        }
        if self.mode == .video {
            return !AssetDataProvider._imageOnlyAssetCollectionSubtypes.contains(subtype)
        }
        return true
    }
    
    private func _loadAllGroups() {
        self.smartAlbumFetchResult.enumerateObjects { (collection, idx, stop) in
            if !self._allowAssetCollectionSubtype(collection.assetCollectionSubtype) {
                return
            }
            
            let key = collection.localIdentifier
            if nil == self.groupDictionary[key] {
                let group = AssetGroup(context: self.option, collection: collection)
                self.groupDictionary[key] = group
                if collection.assetCollectionSubtype == .smartAlbumUserLibrary {
                    self.groupArray.insert(group, at: 0)
                } else {
                    self.groupArray.append(group)
                }
            }
        }
        self.albumFetchResult.enumerateObjects { (collection, idx, stop) in
            if !self._allowAssetCollectionSubtype(collection.assetCollectionSubtype) {
                return
            }

            let key = collection.localIdentifier
            if nil == self.groupDictionary[key] {
                let group = AssetGroup(context: self.option, collection: collection)
                self.groupDictionary[key] = group
                self.groupArray.append(group)
            }
        }
    }
}

extension AssetDataProvider: PHPhotoLibraryChangeObserver {
    private func handlePhotoLibraryChange(changeInstance: PHChange) {
        for group in self.groupArray {
            group.handlePhotoLibraryChange(changeInstance)
        }
    }
    
    public func photoLibraryDidChange(_ changeInstance: PHChange) {
        if Thread.isMainThread {
            self.handlePhotoLibraryChange(changeInstance: changeInstance)
        } else {
            DispatchQueue.main.sync {
                self.handlePhotoLibraryChange(changeInstance: changeInstance)
            }
        }
    }
}

public final class AssetGroup: NSObject, ObservableObject {
    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
    
    public let identifier: String

    private let collection: PHAssetCollection
    public dynamic private(set) var title: String?
    public dynamic private(set) var lastAsset: PHAsset?
    public dynamic private(set) var count: Int = 0
    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]
    
    public let option: AssetDataProvider.Options
    private var thumbnailImageRequestId: PHImageRequestID?
    fileprivate init(context: AssetDataProvider.Options, collection: PHAssetCollection) {
        self.option = context
        self.collection = collection
        self.identifier = collection.localIdentifier
        let options: PHFetchOptions = PHFetchOptions()
        switch context.mode {
        case .image:
            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue)", argumentArray: nil)
        case .video:
            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
        case .imageAndVideo:
            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue) || mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
        }

        let assetFetchResult: PHFetchResult<PHAsset> = PHAsset.fetchAssets(in: self.collection, options: options)
        self.assetFetchResult = assetFetchResult
        self.count = assetFetchResult.count
        self.title = collection.localizedTitle
        self.lastAsset = self.assetFetchResult.lastObject
        super.init()
    }
    
    public func addDataObserver(_ observer: @escaping ObserverClosure, forKey: AnyHashable) {
        self.observers[forKey] = observer
    }
    public func removeDataObserver(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    public override var debugDescription: String {
        get {
            let name: String
            if let localizedTitle = self.collection.localizedTitle {
                name = localizedTitle
            } else {
                name = "nil"
            }
            let dateString: String
            if let startDate = self.collection.startDate {
                dateString = "\(startDate)"
            } else {
                dateString = "nil"
            }
            return "<AssetGroup: \(String(format: "%p", self)), name: \(name), startDate:\(dateString), count:\(self.count)>"
        }
    }

    @discardableResult fileprivate func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
        if let changeDetails: PHFetchResultChangeDetails<PHAsset> = changeInstance.changeDetails(for: self.assetFetchResult) {
            self.assetFetchResult = changeDetails.fetchResultAfterChanges
            self.count = self.assetFetchResult.count
            
            let observers = self.observers
            _ = observers.map { (item) -> Void in
                let closure: ObserverClosure = item.value
                closure(self, changeDetails)
            }
            return true
        } else {
            return false
        }
    }
    
}




