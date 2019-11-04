//
//  GroupDataSource.swift
//  Photo
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos
import UIKit

public protocol AssetItemBuilder {
    associatedtype ItemContext
    associatedtype ItemContent
    func item(asset: PHAsset) -> AssetItem<ItemContext>
}

public protocol Request: AnyObject {
    var isCanceled: Bool {
        get
    }
    var isFinished: Bool {
        get
    }
    func cancel()
}

public typealias ImageRequestResult = RequestResult<UIImage, NSError>

public protocol AssetManager {
    associatedtype ItemContext
    associatedtype ItemContent

    func cachedThumbnailImage(_ identfier: ThumbnailIdentifier) -> UIImage?
    @discardableResult func requestThumbnailImage(asset: AssetItem<ItemContext>, options: ThumbnailOptions, resultHandler: @escaping (_ result: ImageRequestResult) -> Void) -> Request
}

open class AssetProvider<BuilderType, ManagerType, ItemContent, ItemContext> where BuilderType: AssetItemBuilder, BuilderType.ItemContext == ItemContext, BuilderType.ItemContent == ItemContent, ManagerType: AssetManager, ManagerType.ItemContext == ItemContext, ManagerType.ItemContent == ItemContent {
    
    public typealias Provider = AssetProvider<BuilderType, ManagerType, ItemContent, ItemContext>
    public typealias Group = AssetGroup<BuilderType, ManagerType, ItemContext>
    public enum GroupArrayChange {
        case remove([(Int, Group)])
        case insert([(Int, Group)])
    }
    public typealias GroupArrayObserverClosure = (_ entity: Provider, _ changes: [GroupArrayChange]) -> Void

    public let collectionProvider: AssetChangeObserver
    private var groupArrayObservers: [AnyHashable: GroupArrayObserverClosure] = [:]
    public var mode: PhotoMode {
        return self.option.mode
    }
    public var identifier: String {
        return self.option.identifier
    }
    public private(set) var groupArray: [Group] = []
    public private(set) var groupDictionary: [String: Group] = [:]
    
    public let option: ProviderOptions
    public let builder: BuilderType
    public let manager: ManagerType
    
    public init(option: ProviderOptions, builder: BuilderType, manager: ManagerType) {
        self.option = option
        self.builder = builder
        self.manager = manager
        self.collectionProvider = AssetChangeObserver(queue: DispatchQueue.main)
        self._loadAllGroups()
        self.collectionProvider.changeHandler = {[weak self] (changeInstance) -> Void in
            guard let `self` = self else {
                return
            }
            var changes = self._reloadAllGroups()
            for group in self.groupArray {
                group.handlePhotoLibraryChange(changeInstance)
            }
            
            if !changes.isEmpty {
                let observers = self.groupArrayObservers
                for (_, body) in observers {
                    body(self, changes)
                }
            }
        }
    }
    
    public func observeGroupArray(didChange: @escaping GroupArrayObserverClosure, forKey: AnyHashable) {
        self.groupArrayObservers[forKey] = didChange
    }
    public func unobserveGroupArray(forKey: AnyHashable) {
        self.groupArrayObservers.removeValue(forKey: forKey)
    }
    
    private func _reloadAllGroups() -> [GroupArrayChange] {
        let collections = AssetCollectionProvider.fetchAssetCollections(mode: self.option.mode)
        var identifiers: Set<String> = []
        for group in self.groupArray {
            identifiers.insert(group.identifier)
        }
        var removed: [(Int, Group)] = []
        var inserted: [(Int, Group)] = []
        var groups: [Group] = []
        var results: [Group] = []

        for collection in collections {
            let key = collection.localIdentifier
            if identifiers.contains(key) {
                identifiers.remove(key)
            } else {
                let group = AssetGroup(option: self.option, builder: self.builder, manager: self.manager, collection: collection)
                self.groupDictionary[key] = group
                groups.append(group)
            }
        }
        for (index, group) in self.groupArray.enumerated() {
            let key = group.identifier
            if identifiers.contains(key) {
                removed.append((index, group))
            } else {
                results.append(group)
            }
        }
        for group in groups {
            inserted.append((results.count, group))
            results.append(group)
        }
        self.groupArray = results
        var changes: [GroupArrayChange] = []
        if !removed.isEmpty {
            changes.append(.remove(removed))
        }
        if !inserted.isEmpty {
            changes.append(.insert(inserted))
        }
        return changes
    }

    private func _loadAllGroups() {
        let collections = AssetCollectionProvider.fetchAssetCollections(mode: self.option.mode)
        for collection in collections {
            let key = collection.localIdentifier
            if nil == self.groupDictionary[key] {
                let group = AssetGroup(option: self.option, builder: self.builder, manager: self.manager, collection: collection)
                self.groupDictionary[key] = group
                self.groupArray.append(group)
            }
        }
    }
}

public final class AssetContext: NSObject {
    public dynamic var isSelected: Bool = false
    public var cachePath: String? = nil
    public override init() {

    }
}



public final class ProviderContext {
    public private(set) var dictionary: [String: AssetItem<AssetContext>] = [:]
    public init() {

    }
    
//    public func asset(asset: PHAsset) -> AssetItem<>
}

extension ProviderContext: AssetItemBuilder {
    public typealias ItemContext = AssetContext
    public func item(asset: PHAsset) -> AssetItem<AssetContext> {
        if let result = self.dictionary[asset.localIdentifier] {
            return result
        } else {
            return AssetItem<AssetContext>.init(asset: asset, context: ItemContext())
        }
    }
}

public final class AssetItem<ContextType> {
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

public final class AssetGroup<BuilderType, ManagerType, ItemContext> where BuilderType: AssetItemBuilder, BuilderType.ItemContext == ItemContext {
    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
    
    public let identifier: String
    private let collection: PHAssetCollection
    public dynamic private(set) var title: String?
    public dynamic private(set) var lastAsset: AssetItem<ItemContext>?
    public dynamic private(set) var count: Int = 0
    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]
    
    public let option: ProviderOptions
    public let builder: BuilderType
    public let manager: ManagerType
    private var thumbnailImageRequestId: PHImageRequestID?
    public init(option: ProviderOptions, builder: BuilderType, manager: ManagerType, collection: PHAssetCollection) {
        self.option = option
        self.builder = builder
        self.manager = manager
        self.collection = collection
        self.identifier = collection.localIdentifier
        let options: PHFetchOptions = PHFetchOptions()
        switch option.mode {
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
        if let asset = self.assetFetchResult.lastObject {
            self.lastAsset = self.builder.item(asset: asset)
        } else {
            self.lastAsset = nil
        }
    }
    
    public func addDataObserver(_ observer: @escaping ObserverClosure, forKey: AnyHashable) {
        self.observers[forKey] = observer
    }
    public func removeDataObserver(forKey: AnyHashable) {
        self.observers.removeValue(forKey: forKey)
    }
    
//    public var debugDescription: String {
//        get {
//            let name: String
//            if let localizedTitle = self.collection.localizedTitle {
//                name = localizedTitle
//            } else {
//                name = "nil"
//            }
//            let dateString: String
//            if let startDate = self.collection.startDate {
//                dateString = "\(startDate)"
//            } else {
//                dateString = "nil"
//            }
//            return "<Group: \(String(format: "%p", self)), name: \(name), startDate:\(dateString), count:\(self.count)>"
//        }
//    }

    public func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
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
