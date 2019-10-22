//
//  AssetGroupDataSource.swift
//  Photo
//
//  Created by vector on 2019/10/14.
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


public struct AssetGroupDataProviderOptions {
    public static let `default`: AssetGroupDataProviderOptions = {
        var config = AssetGroupDataProviderOptions()
        config.allowedAssetCollectionSubtypes[.smartAlbumUserLibrary] = 0
        config.allowedAssetCollectionSubtypes[.smartAlbumTimelapses] = 1
        config.allowedAssetCollectionSubtypes[.smartAlbumBursts] = 2
        config.allowedAssetCollectionSubtypes[.smartAlbumSelfPortraits] = 3
        config.allowedAssetCollectionSubtypes[.smartAlbumScreenshots] = 4
        config.allowedAssetCollectionSubtypes[.smartAlbumSlomoVideos] = 5
        config.allowedAssetCollectionSubtypes[.smartAlbumVideos] = 6
        config.allowedAssetCollectionSubtypes[.smartAlbumPanoramas] = 7
        config.allowedAssetCollectionSubtypes[.smartAlbumFavorites] = 8
        
        config.allowedAssetCollectionSubtypes[.smartAlbumDepthEffect] = 9
        config.allowedAssetCollectionSubtypes[.smartAlbumLivePhotos] = 10
        config.allowedAssetCollectionSubtypes[.smartAlbumAnimated] = 11
        config.allowedAssetCollectionSubtypes[.smartAlbumLongExposures] = 12
        
        config.allowedAssetCollectionSubtypes[.albumRegular] = 0x8000
        return config
    } ()
    public var allowedAssetCollectionSubtypes: [PHAssetCollectionSubtype: UInt16] = [:]
    public var order: ListOrder = .ascending    
    public init() {
        
    }
}


open class AssetGroupDataProvider: NSObject {
    public let identifier: String
    public private(set) var smartAlbumFetchResult: PHFetchResult<PHAssetCollection>
    public private(set) var albumFetchResult: PHFetchResult<PHAssetCollection>
    
    public private(set) var groupList: UniqueOrderedList<AssetGroup>
    private let options: AssetGroupDataProviderOptions
    
    public init(options: AssetGroupDataProviderOptions = AssetGroupDataProviderOptions.default) {
        self.options = options
        self.identifier = UUID().uuidString
        self.smartAlbumFetchResult = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        self.albumFetchResult = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        super.init()
        PHPhotoLibrary.shared().register(self)
        self.loadCollections()
        self.reload()
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
    
    private func loadCollections() {
        let current: [String: (Int64, PHAssetCollection)] = self.fetchAssetCollections()
        for (key, (originalOrder, item)) in current {
            if self.groupMap[key] == nil {
                let order = AssetOrder(main: self.orderOfAssetCollection(item), sub: originalOrder, detail: "")
                let group = AssetGroup(dataSourceIdentifier: self.identifier, collection: item, order: order)
                self.groupMap[key] = group
            }
        }
    }
    
    
    private class _CollectionSection {
        let order: UInt16
        var items: [PHAssetCollection] = []
        init(order: UInt16) {
            self.order = order
        }
    }
    private class _CollectionTable {
        public let allowedAssetCollectionSubtypes: [PHAssetCollectionSubtype: UInt16] = [:]
        var sections: [PHAssetCollectionSubtype: _CollectionSection] = [:]
        init(allowedAssetCollectionSubtypes: [PHAssetCollectionSubtype: UInt16]) {
            self.allowedAssetCollectionSubtypes = allowedAssetCollectionSubtypes
        }
        
        func append(_ collection: PHAssetCollection) {
            if let section = self.sections[collection.assetCollectionSubtype] {
                section.items.append(collection)
            } else {
                guard let order = self.allowedAssetCollectionSubtypes[collection.assetCollectionSubtype] else {
                    return
                }
                
                let section = _CollectionSection(order: order)
                self.sections[collection.assetCollectionSubtype] = section
                section.items.append(collection)
            }
        }
    }
    private func _fetchAllAssetCollections() -> _CollectionTable {
        var table: _CollectionTable = _CollectionTable(allowedAssetCollectionSubtypes: self.options.allowedAssetCollectionSubtypes)
        self.smartAlbumFetchResult.enumerateObjects { (item, idx, stop) in
            table.append(item)
        }
        self.albumFetchResult.enumerateObjects { (item, idx, stop) in
            table.append(item)
        }
        return table
    }
    private func reload() {
//        var groups: [AssetGroup] = self.groupMap.map { (item) -> AssetGroup in
//            return item.value
//        }
//        groups.sort { (lhs, rhs) -> Bool in
//            return lhs.order < rhs.order
//        }
//        self.groups = groups
    }
    private func groupsDidChange(removed: [String: AssetGroup], inserted: [String: AssetGroup], updated: [String: AssetGroup]) {
        
    }
}


extension AssetGroupDataProvider: PHPhotoLibraryChangeObserver {
    
    private func handlePhotoLibraryChange(changeInstance: PHChange) {
        if let change = changeInstance.changeDetails(for: self.albumFetchResult) {
            self.albumFetchResult = change.fetchResultAfterChanges
        }
        if let change = changeInstance.changeDetails(for: self.smartAlbumFetchResult) {
            self.smartAlbumFetchResult = change.fetchResultAfterChanges
        }
        
        var new: [String: AssetGroup] = [:]

        let current: [String: (Int64, PHAssetCollection)] = self.fetchAssetCollections()
        let old: [String: AssetGroup] = self.groupMap
        var removed: [String: AssetGroup] = [:]
        var inserted: [String: AssetGroup] = [:]
        var updated: [String: AssetGroup] = [:]
        for (key, item) in old {
            if current[key] == nil {
                removed[key] = item
            } else {
                new[key] = item
            }
        }
        for (key, (originalOrder, item)) in current {
            if let group = old[key] {
                group.order.sub = originalOrder
            } else {
                let order = AssetOrder(main: self.orderOfAssetCollection(item), sub: originalOrder, detail: "")
                let group = AssetGroup(dataSourceIdentifier: self.identifier, collection: item, order: order)
                new[key] = group
                inserted[key] = group
            }
        }
        self.groupMap = new
        
        _ = self.groupMap.mapValues { (group) -> Void in
            if group.handlePhotoLibraryChange(changeInstance) {
                updated[group.identifier] = group
            }
        }
        
        self.groupsDidChange(removed: removed, inserted: inserted, updated:updated)

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




public protocol AssetProtocol {
    var identifier: String {
        get
    }
    
}





public final class AssetGroup: NSObject, UniqueOrderedListElement {
    public typealias UniqueOrder = AssetOrder
    public typealias UniqueIdentifier = String

    
    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
    
    let dataSourceIdentifier: String
    let identifier: String
    fileprivate var order: UniqueOrder
    
    public var uniqueOrder: UniqueOrder {
        return self.order
    }
    public var uniqueIdentifier: UniqueIdentifier {
        return self.identifier
    }
    
    public func observeUniqueOrder(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String) {}
    public func unobserveUniqueOrder(forKey:String) {}

    public func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String) {}
    public func unobserveContent(forKey:String) {}
    
    

    private let collection: PHAssetCollection
    public private(set) var count: Int = 0
    var title: String? {
        return self.collection.localizedTitle
    }
    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]

    fileprivate init(dataSourceIdentifier: String, collection: PHAssetCollection, order: AssetOrder) {
        self.dataSourceIdentifier = dataSourceIdentifier
        self.collection = collection
        self.identifier = collection.localIdentifier
        self.order = order
        let options: PHFetchOptions = PHFetchOptions()
        let assetFetchResult: PHFetchResult<PHAsset> = PHAsset.fetchAssets(in: self.collection, options: options)
        self.assetFetchResult = assetFetchResult
        self.count = assetFetchResult.count
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
            return "<AssetGroup: \(String(format: "%p", self)), name: \(name), startDate:\(dateString)>"
        }
    }

    fileprivate func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
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




