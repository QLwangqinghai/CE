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

fileprivate final class AssetContext: NSObject {
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
    
    fileprivate override init() {

    }
}


open class BaseAssetGroupDataProvider: NSObject {
    public let identifier: String
    public private(set) var smartAlbumFetchResult: PHFetchResult<PHAssetCollection>
    public private(set) var albumFetchResult: PHFetchResult<PHAssetCollection>
    public private(set) var groupMap: [String: AssetGroup] = [:]
    
    public override init() {
        self.identifier = UUID().uuidString
        self.smartAlbumFetchResult = PHAssetCollection.fetchAssetCollections(with: .smartAlbum, subtype: .any, options: nil)
        self.albumFetchResult = PHAssetCollection.fetchAssetCollections(with: .album, subtype: .any, options: nil)
        super.init()
        PHPhotoLibrary.shared().register(self)
        self.loadCollections()
    }
    deinit {
        PHPhotoLibrary.shared().unregisterChangeObserver(self)
    }
    
    private func loadCollections() {
        let current: [String: (UInt, PHAssetCollection)] = self.fetchAssetCollections()
        for (key, (originalOrder, item)) in current {
            if self.groupMap[key] == nil {
                let order = AssetGroup.Order(main: self.orderOfAssetCollection(item), originalOrder: originalOrder)
                let group = AssetGroup(dataSourceIdentifier: self.identifier, collection: item, order: order)
                self.groupMap[key] = group
            }
        }
    }
    
    public func fetchAssetCollections() -> [String: (UInt, PHAssetCollection)] {
        var map: [String: (UInt, PHAssetCollection)] = [:]
        self.smartAlbumFetchResult.enumerateObjects { (item, idx, stop) in
            map[item.localIdentifier] = (UInt(idx), item)
        }
        let base = UInt(Int.max) + 1
        self.albumFetchResult.enumerateObjects { (item, idx, stop) in
            map[item.localIdentifier] = (UInt(idx) + base, item)
        }
        return map.filter { (arg0) -> Bool in
            let (_, (_, item)) = arg0
            return self.filterAssetCollection(item)
        }
    }

    open func filterAssetCollection(_ collection: PHAssetCollection) -> Bool {
        return true
    }
    open func groupsDidChange(inserted: [String: AssetGroup], removed: [String: AssetGroup]) {
    }
    open func orderOfAssetCollection(_ collection: PHAssetCollection) -> UInt64 {
        return 0
    }
}


extension BaseAssetGroupDataProvider: PHPhotoLibraryChangeObserver {
    
    private func handlePhotoLibraryChange(changeInstance: PHChange) {
        if let change = changeInstance.changeDetails(for: self.albumFetchResult) {
            self.albumFetchResult = change.fetchResultAfterChanges
        }
        if let change = changeInstance.changeDetails(for: self.smartAlbumFetchResult) {
            self.smartAlbumFetchResult = change.fetchResultAfterChanges
        }
        
        var new: [String: AssetGroup] = [:]

        let current: [String: (UInt, PHAssetCollection)] = self.fetchAssetCollections()
        let old: [String: AssetGroup] = self.groupMap
        var removed: [String: AssetGroup] = [:]
        var inserted: [String: AssetGroup] = [:]
        
        for (key, item) in old {
            if current[key] == nil {
                removed[key] = item
            } else {
                new[key] = item
            }
        }
        for (key, (originalOrder, item)) in current {
            if let group = old[key] {
                group.order.originalOrder = originalOrder
            } else {
                let order = AssetGroup.Order(main: self.orderOfAssetCollection(item), originalOrder: originalOrder)
                let group = AssetGroup(dataSourceIdentifier: self.identifier, collection: item, order: order)
                new[key] = group
                inserted[key] = group
            }
        }
        self.groupMap = new
        self.groupsDidChange(inserted: inserted, removed: removed)
        
        _ = self.groupMap.mapValues { (group) -> Void in
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




public protocol AssetProtocol {
    var identifier: String {
        get
    }
    
}

public final class AssetItem: NSObject {

    
    
}



public struct AssetGroupDataProviderOptions {
    public static let `default`: AssetGroupDataProviderOptions = {
        var config = AssetGroupDataProviderOptions()
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
    public var filter: (PHAssetCollection) -> Bool = { (item) -> Bool in
        if AssetGroupDataProviderOptions.defaultTypeFilterSet.contains(item.assetCollectionSubtype) {
            return true
        } else {
            return false
        }
    }
    public var typeOrder: TypePriority = .smartAlbum
    
    //
    public var subTypeOrderConfig: [PHAssetCollectionSubtype: UInt16] = [:]
    
    public init() {
        
    }
    
    
    
}


public enum AssetChangeType: Int {
    case insert
    case remove
    case update
}
public struct AssetChange {
    let type: AssetChangeType
    let groups: [(Int, AssetGroup)]
}

public class AssetGroupDataProviderObserver: NSObject {
    public typealias GroupsDidReloadClosure = (_ provider: AssetGroupDataProvider) -> Void
    public typealias GroupsDidChangeClosure = (_ provider: AssetGroupDataProvider, _ changes: [AssetChange]) -> Void

    public let groupsDidReload: GroupsDidReloadClosure
    public let groupsDidChange: GroupsDidChangeClosure

    public init(groupsDidReload: @escaping GroupsDidReloadClosure, groupsDidChange: @escaping GroupsDidChangeClosure) {
        self.groupsDidChange = groupsDidChange
        self.groupsDidReload = groupsDidReload
        super.init()
    }
}

public protocol AssetGroupDataProviderDelegate: class {
    func groupsDidReload(provider: AssetGroupDataProvider)
    func groupsDidChange(provider: AssetGroupDataProvider, changes: [AssetChange])
}


public final class AssetGroupDataProvider: BaseAssetGroupDataProvider {
    public private(set) var groups: [AssetGroup] = []
    private let options: AssetGroupDataProviderOptions
    
    public private(set) var observers: [AnyHashable: AssetGroupDataProviderObserver] = [:]
    public init(options: AssetGroupDataProviderOptions = AssetGroupDataProviderOptions.default) {
        self.options = options
        super.init()
        self.reload()
    }
    
    public func addDataObserver(_ observer: AssetGroupDataProviderObserver, forKey: AnyHashable) {
        self.observers[forKey] = observer
    }
    public func removeDataObserver(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
    private func reload() {
        var groups: [AssetGroup] = self.groupMap.map { (item) -> AssetGroup in
            return item.value
        }
        groups.sort { (lhs, rhs) -> Bool in
            return lhs.order < rhs.order
        }
        self.groups = groups
        
        print(groups)
    }
    
    public override func filterAssetCollection(_ collection: PHAssetCollection) -> Bool {
        return self.options.filter(collection)
    }
    public override func groupsDidChange(inserted: [String: AssetGroup], removed: [String: AssetGroup]) {
        super.groupsDidChange(inserted: inserted, removed: removed)
        
        func goodInsertIndex(groups: [AssetGroup], group: AssetGroup) -> Int {
            guard !groups.isEmpty else {
                return 0
            }
            for (index, item) in groups.enumerated() {
                if group.order < item.order {
                    return index
                }
            }
            return groups.count
        }
        
        func onError() {
            self.reload()
            let observers = self.observers
            for (_, observer) in observers {
                observer.groupsDidReload(self)
            }
        }
        
        var changes: [AssetChange] = []
        if !removed.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            let oldGroups = self.groups
            var newGroups: [AssetGroup] = []
            for (index, item) in oldGroups.enumerated() {
                if removed[item.identifier] != nil {
                    groups.append((index, item))
                } else {
                    newGroups.append(item)
                }
            }
            self.groups = newGroups
            if !groups.isEmpty {
                let change = AssetChange(type: .remove, groups: groups)
                changes.append(change)
            }
        }
        
        //heath check
        var orderError = false
        let groupsCountAfterRemove = self.groups.count
        if groupsCountAfterRemove >= 2 {
            for index in 0 ..< self.groups.count - 2 {
                if self.groups[index + 1].order < self.groups[index].order {
                    orderError = true
                    break
                }
            }
        }
        
        if orderError {
            onError()
            return
        }
        
        if !inserted.isEmpty {
            var groups: [(Int, AssetGroup)] = []
            var insertGroups = inserted.map { (item) -> AssetGroup in
                return item.value
            }
            insertGroups.sort { (lhs, rhs) -> Bool in
                return rhs.order < rhs.order
            }
            
            for item in insertGroups {
                if self.groups.firstIndex(of: item) != nil {
                    onError()
                    return
                }
                let index = goodInsertIndex(groups: self.groups, group: item)
                self.groups.insert(item, at: index)
                groups.append((index, item))
            }
            
            if !groups.isEmpty {
                let change = AssetChange(type: .insert, groups: groups)
                changes.append(change)
            }
        }
        if !changes.isEmpty {
            let observers = self.observers
            for (_, observer) in observers {
                observer.groupsDidChange(self, changes)
            }
        }
    }
    public override func orderOfAssetCollection(_ collection: PHAssetCollection) -> UInt64 {
        var typeOrder: UInt64
        if let v = self.options.subTypeOrderConfig[collection.assetCollectionSubtype] {
            typeOrder = UInt64(v) << 48
        } else {
            typeOrder = 0xffff_0000_0000_0000
        }
        return typeOrder
    }
}

//open class BaseAssetDataProvider: NSObject {
//    public init(items: PHFetchResult<PHAsset>) {
//        self.dataSourceIdentifier = dataSourceIdentifier
//        self.collection = collection
//        self.identifier = collection.localIdentifier
//        self.order = order
//        super.init()
//        PHPhotoLibrary.shared().register(self)
//        let options = PHFetchOptions()
//
//        let items: PHFetchResult<PHAsset> = PHAsset.fetchAssets(in: self.collection, options: options)
//        self.collection
//
//    }
//}

public final class AssetGroup: NSObject {
    struct Order {
        var main: UInt64
        var originalOrder: UInt
        static func < (lhs: Order, rhs: Order) -> Bool {
            return lhs.main == rhs.main ? lhs.originalOrder < rhs.originalOrder : lhs.main < rhs.main
        }
    }
    
    let dataSourceIdentifier: String
    let identifier: String
    fileprivate var order: Order

    private let collection: PHAssetCollection
    dynamic var count: Int = 0
    var title: String? {
        return self.collection.localizedTitle
    }

    private let assetFetchResult: PHFetchResult<PHAsset>
    
    fileprivate init(dataSourceIdentifier: String, collection: PHAssetCollection, order: Order) {
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

    fileprivate func handlePhotoLibraryChange(_ changeInstance: PHChange) {
        if let changeDetails = changeInstance.changeDetails(for: self.assetFetchResult) {
            self.count = self.assetFetchResult.count
        }
    }

    
}

public final class AssetList: NSObject {
    public let group: AssetGroup
    
    public init(group: AssetGroup) {
        self.group = group
        super.init()
    }
    
    
    
    
    
    
    
    
    
    
}


