//
//  AssetGroupDataSource.swift
//  Photo
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
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
 albumSyncedEvent //使用 iTunes 从 Photos 照片库或者 iPhoto 照片库同步过来的事件。然而，在iTunes 12 以及iOS 9.0 beta4上，选用该类型没法获取同步的事件相册，而必须使用AlbumSyncedAlbum。
 albumSyncedFaces //使用 iTunes 从 Photos 照片库或者 iPhoto 照片库同步的人物相册。
 albumSyncedAlbum //做了 AlbumSyncedEvent 应该做的事
 albumImported //从相机或是外部存储导入的相册，完全没有这方面的使用经验，没法验证。
 albumMyPhotoStream //用户的 iCloud 照片流
 albumCloudShared //用户使用 iCloud 共享的相册
 
 smartAlbumGeneric //文档解释为非特殊类型的相册，主要包括从 iPhoto 同步过来的相册。由于本人的 iPhoto 已被 Photos 替代，无法验证。不过，在我的 iPad mini 上是无法获取的，而下面类型的相册，尽管没有包含照片或视频，但能够获取到。
 smartAlbumPanoramas //全景照片 相机拍摄的全景照片
 smartAlbumVideos //视频 相机拍摄的视频
 smartAlbumFavorites //个人收藏 收藏文件夹
 smartAlbumTimelapses //延时摄影 延时视频文件夹，同时也会出现在视频文件夹中
 smartAlbumAllHidden //已隐藏 包含隐藏照片或视频的文件夹
 smartAlbumRecentlyAdded //最近项目 相机近期拍摄的照片或视频
 smartAlbumBursts //连拍快照 连拍模式拍摄的照片
 smartAlbumSlomoVideos //慢动作 Slomo 是 slow motion 的缩写，高速摄影慢动作解析，在该模式下，iOS 设备以120帧拍摄。
 smartAlbumUserLibrary //最近项目 相机相册，所有相机拍摄的照片或视频都会出现在该相册中，而且使用其他应用保存的照片也会出现在这里。
 any //全部类型

     @available(iOS 9, *)
     case smartAlbumSelfPortraits //自拍

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

 }
 */

public protocol AssetProtocol {
    var identifier: String {
        get
    }
    
}

public final class AssetItem: NSObject {

    
    
}



public struct AssetDataProviderOptions {
    public static let `default`: AssetDataProviderOptions = {
        var config = AssetDataProviderOptions()
        config.subTypeOrderConfig[.smartAlbumUserLibrary] = 0
        config.subTypeOrderConfig[.smartAlbumTimelapses] = 1
        config.subTypeOrderConfig[.smartAlbumBursts] = 2
        config.subTypeOrderConfig[.smartAlbumSelfPortraits] = 3
        config.subTypeOrderConfig[.smartAlbumScreenshots] = 4
        config.subTypeOrderConfig[.smartAlbumSlomoVideos] = 5
        config.subTypeOrderConfig[.smartAlbumVideos] = 6
        config.subTypeOrderConfig[.smartAlbumPanoramas] = 7
        config.subTypeOrderConfig[.smartAlbumFavorites] = 8
        
        config.subTypeOrderConfig[.smartAlbumDepthEffect] = 9
        config.subTypeOrderConfig[.smartAlbumLivePhotos] = 10
        config.subTypeOrderConfig[.smartAlbumAnimated] = 11
        config.subTypeOrderConfig[.smartAlbumLongExposures] = 12
        
        config.subTypeOrderConfig[.albumRegular] = 0x8000
        return config
    } ()

    public enum TypePriority: Int {
        case smartAlbum
        case album
    }
    
    public static let defaultTypeFilterSet: Set<PHAssetCollectionSubtype> = {
        var set = Set<PHAssetCollectionSubtype>()
        set.insert(.smartAlbumUserLibrary)
        set.insert(.smartAlbumUserLibrary)
        set.insert(.smartAlbumTimelapses)
        set.insert(.smartAlbumBursts)
        set.insert(.smartAlbumSelfPortraits)
        set.insert(.smartAlbumScreenshots)
        set.insert(.smartAlbumSlomoVideos)
        set.insert(.smartAlbumVideos)
        set.insert(.smartAlbumPanoramas)
        set.insert(.smartAlbumFavorites)
        set.insert(.smartAlbumDepthEffect)
        set.insert(.smartAlbumLivePhotos)
        set.insert(.smartAlbumAnimated)
        set.insert(.smartAlbumLongExposures)
        set.insert(.albumRegular)
        return set
    }()
    public var filter: (PHAssetCollection) -> PHAssetCollection? = { (item) -> PHAssetCollection? in
        if AssetDataProviderOptions.defaultTypeFilterSet.contains(item.assetCollectionSubtype) {
            return item
        } else {
            return nil
        }
    }
    public var typeOrder: TypePriority = .smartAlbum
    
    //
    public var subTypeOrderConfig: [PHAssetCollectionSubtype: UInt16] = [:]
    
    
    public init() {
        
    }
    
    
    
}

public final class AssetGroup: NSObject {
    let dataSourceIdentifier: String
    let identifier: String
    internal var order: UInt64 = 0
//    let sequence: Int

    private let collection: PHAssetCollection
    
    public init(dataSourceIdentifier: String, collection: PHAssetCollection) {
        self.dataSourceIdentifier = dataSourceIdentifier
        self.collection = collection
        self.identifier = collection.localIdentifier
        super.init()
        PHPhotoLibrary.shared().register(self)
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
}

extension AssetGroup: PHPhotoLibraryChangeObserver {
    public func photoLibraryDidChange(_ changeInstance: PHChange) {
//        let fetchResultChanges = changeInstance.changeDetails(for: self.fetchResult)
//        print("changeInstance:\(changeInstance) \(fetchResultChanges)")
    }
}

enum AssetChangeType: Int {
    case insert
    case remove
    case update
}

public protocol AssetDataProviderDelegate: class {
    
    
    
}


public final class AssetDataProvider: BaseAssetDataProvider {
    private var groups: [AssetGroup] = []
    

}

extension AssetDataProvider {
    public override func photoLibraryDidChange(_ changeInstance: PHChange) {
        super.photoLibraryDidChange(changeInstance)
        let smartAlbumCollectionChanges = changeInstance.changeDetails(for: self.smartAlbumFetchResult)
        let albumCollectionChanges = changeInstance.changeDetails(for: self.albumFetchResult)
        print("changeInstance:\(changeInstance) \(smartAlbumCollectionChanges) \(albumCollectionChanges)")
        
        albumCollectionChanges?.changedIndexes
        
//        PHFetchResultChangeDetails *collectionChanges = [changeInstance changeDetailsForFetchResult:self.assetsFetchResults];
//        if (collectionChanges) {
//            if ([collectionChanges hasIncrementalChanges]) {
//                //监听相册视频的增删
//                //增加了
//                if (collectionChanges.insertedObjects.count > 0) {
//                    NSMutableArray *mArr = [[NSMutableArray alloc] initWithArray:collectionChanges.insertedObjects];
//                    [self getAlbumVideoWithBehaviorType:ALBUMVIDEOBEHAVIOR_INSTER PHObjectsArr:mArr];
//                }
//                //删除了
//                if (collectionChanges.removedObjects.count > 0) {
//
//                    NSMutableArray *mArr = [[NSMutableArray alloc] initWithArray:collectionChanges.removedObjects];
//                    [self getAlbumVideoWithBehaviorType:ALBUMVIDEOBEHAVIOR_REMOVE PHObjectsArr:mArr];
//                }
//                /**监听完一次更新一下监听对象*/
//                self.assetsFetchResults = [PHAsset fetchAssetsWithMediaType:PHAssetMediaTypeVideo options:[self getFetchPhotosOptions]];
//            }
//        }
    }
}







public class BaseAssetDataProvider: NSObject {
    let identifier: String
    let smartAlbumFetchResult: PHFetchResult<PHAssetCollection>
    let albumFetchResult: PHFetchResult<PHAssetCollection>
    private let options: AssetDataProviderOptions
    private var groupMap: [String: AssetGroup] = [:]
    
    public init(options: AssetDataProviderOptions = AssetDataProviderOptions.default) {
        self.options = options
        self.identifier = UUID().uuidString
        self.smartAlbumFetchResult = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        self.albumFetchResult = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        super.init()
        PHPhotoLibrary.shared().register(self)
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
    
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
    
    static func testSmartAlbum() {
        print("\n\n\n")
        print("testSmartAlbum")
        var c : PHAssetCollection? = nil
        
        let a = CFAbsoluteTimeGetCurrent();
        let result = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        let b = CFAbsoluteTimeGetCurrent();
        print("b-a \(b-a)")
        
        result.enumerateObjects { (item, idx, stop) in
            let subType = self.subTypeMap[item.assetCollectionSubtype]
            print("idx:\(idx) itemType:\(subType) localizedTitle:\(item.localizedTitle) item:\(item)")
        }
        print(result)
        
    }
    static func testAlbum() {
        print("\n\n\n")
        print("testAlbum")
        var c : PHAssetCollection? = nil
        let result = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        result.enumerateObjects { (item, idx, stop) in
            let subType = self.subTypeMap[item.assetCollectionSubtype]
            var date = ""
            if let d = item.startDate {
                date = "\(d)"
            }
            print("idx:\(idx) itemType:\(subType) \(date) localizedTitle:\(item.localizedTitle) item:\(item)")
        }
        print(result)
    }
    static func test() {

        self.testSmartAlbum()
        self.testAlbum()
    }

    
    
    
}

extension BaseAssetDataProvider: PHPhotoLibraryChangeObserver {
    public func photoLibraryDidChange(_ changeInstance: PHChange) {
        let smartAlbumCollectionChanges = changeInstance.changeDetails(for: self.smartAlbumFetchResult)
        let albumCollectionChanges = changeInstance.changeDetails(for: self.albumFetchResult)
        print("changeInstance:\(changeInstance) \(smartAlbumCollectionChanges) \(albumCollectionChanges)")
        
        albumCollectionChanges?.changedIndexes
        
//        PHFetchResultChangeDetails *collectionChanges = [changeInstance changeDetailsForFetchResult:self.assetsFetchResults];
//        if (collectionChanges) {
//            if ([collectionChanges hasIncrementalChanges]) {
//                //监听相册视频的增删
//                //增加了
//                if (collectionChanges.insertedObjects.count > 0) {
//                    NSMutableArray *mArr = [[NSMutableArray alloc] initWithArray:collectionChanges.insertedObjects];
//                    [self getAlbumVideoWithBehaviorType:ALBUMVIDEOBEHAVIOR_INSTER PHObjectsArr:mArr];
//                }
//                //删除了
//                if (collectionChanges.removedObjects.count > 0) {
//
//                    NSMutableArray *mArr = [[NSMutableArray alloc] initWithArray:collectionChanges.removedObjects];
//                    [self getAlbumVideoWithBehaviorType:ALBUMVIDEOBEHAVIOR_REMOVE PHObjectsArr:mArr];
//                }
//                /**监听完一次更新一下监听对象*/
//                self.assetsFetchResults = [PHAsset fetchAssetsWithMediaType:PHAssetMediaTypeVideo options:[self getFetchPhotosOptions]];
//            }
//        }
    }
}
