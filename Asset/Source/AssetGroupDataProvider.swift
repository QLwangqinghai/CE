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
    }()
    
    
    public var allowedAssetCollectionSubtypes: [PHAssetCollectionSubtype: UInt16] = [:]
    
    //value < 0x8000, use15bit
    fileprivate let defaultOrder: [PHAssetCollectionSubtype: UInt16] = {
        var map = [PHAssetCollectionSubtype: UInt16]()
        map[.smartAlbumUserLibrary] = 0
        map[.smartAlbumTimelapses] = 1
        map[.smartAlbumBursts] = 2
        map[.smartAlbumSelfPortraits] = 3
        map[.smartAlbumScreenshots] = 4
        map[.smartAlbumSlomoVideos] = 5
        map[.smartAlbumVideos] = 6
        map[.smartAlbumPanoramas] = 7
        map[.smartAlbumFavorites] = 8
        
        map[.smartAlbumDepthEffect] = 9
        map[.smartAlbumLivePhotos] = 10
        map[.smartAlbumAnimated] = 11
        map[.smartAlbumLongExposures] = 12
        map[.albumRegular] = 0x4000
        return map
    } ()
    
    public var order: ListOrder = .ascending
    public init() {
        
    }

    fileprivate func order(ofSubtype: PHAssetCollectionSubtype) -> Int32? {
        if let v = self.allowedAssetCollectionSubtypes[ofSubtype] {
            var order: Int32 = (Int32(v) << 15)
            if let sv = self.defaultOrder[ofSubtype] {
                order += Int32(sv)
            }
            return order
        } else {
            return nil
        }
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
        self.groupList = UniqueOrderedList<AssetGroup>()
        super.init()
        PHPhotoLibrary.shared().register(self)
        self.loadCollections()
        self.reload()
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
    
    private func loadCollections() {
        let table: _CollectionTable = self._fetchAllAssetCollections()
        var inserted: [AssetGroup] = []
        var groupIdentifiers: Set<String> = []
        for (_, section) in table.sections {
            for (index, collection) in section.items.enumerated() {
                let sequence = Int64(index)
                var time: Int64 = 0
                if let startDate = collection.startDate {
                    time = Int64(startDate.timeIntervalSince1970 * 1000.0)
                }
                groupIdentifiers.insert(collection.localIdentifier)
                if let group = self.groupList.item(forKey: collection.localIdentifier) {
                    group.order.sequence = sequence
                    group.order.time = time
                } else {
                    let order = AssetOrder(main: Int64(section.order), time: time, sequence: sequence)
                    let group = AssetGroup(dataSourceIdentifier: self.identifier, collection: collection, order: order)
                    inserted.append(group)
                }
            }
        }
        self.groupList.batch(filter: { (group) -> Bool in
            return groupIdentifiers.contains(group.identifier)
        }, inserts: inserted)
    }
    
    
    private class _CollectionSection {
        let order: Int32
        var items: [PHAssetCollection] = []
        init(order: Int32) {
            self.order = order
        }
    }
    private class _CollectionTable {
        public let options: AssetGroupDataProviderOptions
        var sections: [PHAssetCollectionSubtype: _CollectionSection] = [:]
        init(options: AssetGroupDataProviderOptions) {
            self.options = options
        }
        
        func append(_ collection: PHAssetCollection) {
            if let section = self.sections[collection.assetCollectionSubtype] {
                section.items.append(collection)
            } else {
                guard let order = self.options.order(ofSubtype: collection.assetCollectionSubtype) else {
                    return
                }
                
                let section = _CollectionSection(order: order)
                self.sections[collection.assetCollectionSubtype] = section
                section.items.append(collection)
            }
        }
    }
    private func _fetchAllAssetCollections() -> _CollectionTable {
        let table: _CollectionTable = _CollectionTable(options: self.options)
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
    private func groupsDidChange(removed: [PHAssetCollection], inserted: [PHAssetCollection], updated: [AssetGroup]) {
        
    }
}


extension AssetGroupDataProvider: PHPhotoLibraryChangeObserver {
    
    private func handlePhotoLibraryChange(changeInstance: PHChange) {
        var removed: [PHAssetCollection] = []
        var inserted: [PHAssetCollection] = []

        if let change = changeInstance.changeDetails(for: self.albumFetchResult) {
            self.albumFetchResult = change.fetchResultAfterChanges

            for collection in change.removedObjects {
                removed.append(collection)
            }
            for collection in change.insertedObjects {
                inserted.append(collection)
            }
        }
        if let change = changeInstance.changeDetails(for: self.smartAlbumFetchResult) {
            self.smartAlbumFetchResult = change.fetchResultAfterChanges
            for collection in change.removedObjects {
                removed.append(collection)
            }
            for collection in change.insertedObjects {
                inserted.append(collection)
            }
        }
        
        let updated = self.groupList.list.filter { (group) -> Bool in
            return group.handlePhotoLibraryChange(changeInstance)
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



