//
//  AssetCollectionProvider.swift
//  Asset
//
//  Created by vector on 2019/11/1.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

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
 
 smartAlbumGeneric //文档解释为非特殊类型的相册，主要包括从 iPhoto 同步过来的相册。
 smartAlbumPanoramas //全景照片[图片] 相机拍摄的全景照片
 smartAlbumVideos //视频[视频] 相机拍摄的视频
 smartAlbumFavorites //个人收藏[视频 + 图片] 收藏文件夹
 smartAlbumTimelapses //延时摄影[视频] 延时视频文件夹，同时也会出现在视频文件夹中
 smartAlbumAllHidden //已隐藏 包含隐藏照片或视频的文件夹
 smartAlbumRecentlyAdded //  最近项目[视频 + 图片] 相机近期拍摄的照片或视频
 smartAlbumBursts //连拍快照[图片] 连拍模式拍摄的照片
 smartAlbumSlomoVideos //慢动作[视频] Slomo 是 slow motion 的缩写，高速摄影慢动作解析，在该模式下，iOS 设备以120帧拍摄。
 smartAlbumUserLibrary //最近项目[视频 + 图片] 相机相册，所有相机拍摄的照片或视频都会出现在该相册中，而且使用其他应用保存的照片也会出现在这里。
 any //全部类型

     @available(iOS 9, *)
     case smartAlbumSelfPortraits //自拍[视频 + 图片]

     @available(iOS 9, *)
     case smartAlbumScreenshots //截屏[?]

     @available(iOS 10.2, *)
     case smartAlbumDepthEffect //人像[图片]

     @available(iOS 10.3, *)
     case smartAlbumLivePhotos //实况照片[图片]

     @available(iOS 11, *)
     case smartAlbumAnimated //动图[图片]
 
     @available(iOS 11, *)
     case smartAlbumLongExposures //长曝光[图片]
 
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

public final class AssetChangeObserver: NSObject {
    public var changeHandler: ((_ changeInstance: PHChange) -> Void)?
    public let queue: DispatchQueue
    public init(queue: DispatchQueue = DispatchQueue.main) {
        self.queue = queue
        super.init()
        PHPhotoLibrary.shared().register(self)
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
}

extension AssetChangeObserver: PHPhotoLibraryChangeObserver {
    private func handlePhotoLibraryChange(changeInstance: PHChange) {
        if let handler = self.changeHandler {
            handler(changeInstance)
        }
    }
    public func photoLibraryDidChange(_ changeInstance: PHChange) {
        if self.queue == DispatchQueue.main && Thread.isMainThread {
            self.handlePhotoLibraryChange(changeInstance: changeInstance)
        } else {
            self.queue.async {
                self.handlePhotoLibraryChange(changeInstance: changeInstance)
            }
        }
    }
}




public struct AssetCollectionProvider {
    private static func _allowAssetCollection(_ collection: PHAssetCollection, mode: PhotoMode) -> Bool {
        let subtype = collection.assetCollectionSubtype
        if subtype == .smartAlbumAllHidden {
            return false
        }
        if mode == .image {
            return !AssetCollectionProvider.videoOnlySubtypes.contains(subtype)
        }
        if mode == .video {
            return !AssetCollectionProvider.imageOnlySubtypes.contains(subtype)
        }
        return true
    }
    
    public static func fetchAssetCollections(mode: PhotoMode) -> [PHAssetCollection] {
        let smartAlbumFetchResult = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        let albumFetchResult = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        var result: [PHAssetCollection] = []
        smartAlbumFetchResult.enumerateObjects { (collection, idx, stop) in
            if !self._allowAssetCollection(collection, mode: mode) {
                return
            }
            if collection.assetCollectionSubtype == .smartAlbumUserLibrary {
                result.insert(collection, at: 0)
            } else {
                result.append(collection)
            }
        }
        albumFetchResult.enumerateObjects { (collection, idx, stop) in
            if !self._allowAssetCollection(collection, mode: mode) {
                return
            }
            result.append(collection)
        }
        return result
    }
    
    public static let videoOnlySubtypes: Set<PHAssetCollectionSubtype> = {
        var types: Set<PHAssetCollectionSubtype> = [.smartAlbumTimelapses, .smartAlbumSlomoVideos, .smartAlbumVideos]
        return types
    }()
    
    public static let imageOnlySubtypes: Set<PHAssetCollectionSubtype> = {
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
}
