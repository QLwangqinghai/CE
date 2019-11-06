import UIKit

import Photos
import UIKit

public enum PhotoMode {
    case image
    case video
    case imageAndVideo
}

public final class ProviderOptions {
    //Thumbnail
    public let identifier: String
    public let mode: PhotoMode

    public init(mode: PhotoMode = .imageAndVideo) {
        self.mode = mode
        self.identifier = UUID().uuidString
    }
}


//
//  Base.swift
//  Core
//
//  Created by vector on 2019/11/4.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation
import UIKit

public enum RequestResult<SuccessType, ErrorType> {
    case success(SuccessType)
    case error(ErrorType)
    case cancel
}

public enum ImageContentMode: Int {
    case aspectFit
    case aspectFill
}

public struct ThumbnailOptions: Equatable {
    public let width: Int
    public let height: Int
    public let contentMode: ImageContentMode

    public init(width: Int, height: Int, contentMode: ImageContentMode) {
        self.width = width
        self.height = height
        self.contentMode = contentMode
    }
    public init(size: CGSize, contentMode: ImageContentMode) {
        let scale = UIScreen.main.scale
        self.width = Int(Double(size.width * scale + 0.5))
        self.height = Int(Double(size.height * scale + 0.5))
        self.contentMode = contentMode
    }
    public static func == (lhs: ThumbnailOptions, rhs: ThumbnailOptions) -> Bool {
        return lhs.width == rhs.width && lhs.height == rhs.height && lhs.contentMode == rhs.contentMode
    }
}

public class ThumbnailIdentifier: Hashable {
    public let identifier: AnyHashable
    public let options: ThumbnailOptions
    public init(identifier: AnyHashable, options: ThumbnailOptions) {
        self.identifier = identifier
        self.options = options
    }

    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.identifier)
        hasher.combine(self.options.width)
    }
    
    public static func == (lhs: ThumbnailIdentifier, rhs: ThumbnailIdentifier) -> Bool {
        return lhs.identifier == rhs.identifier && lhs.options == rhs.options
    }
    public static func == (lhs: ThumbnailIdentifier?, rhs: ThumbnailIdentifier) -> Bool {
        guard let lhsValue = lhs else {
            return false
        }
        return lhsValue == rhs
    }
    public static func == (lhs: ThumbnailIdentifier, rhs: ThumbnailIdentifier?) -> Bool {
        guard let rhsValue = rhs else {
            return false
        }
        return lhs == rhsValue
    }
}


//
//  GroupDataSource.swift
//  Photo
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos
import UIKit

public protocol AssetBuilder {
    associatedtype ItemType: AssetItem
    func item(asset: ItemType.ContentType) -> ItemType
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

public protocol AssetDataSource: class {
    associatedtype ItemType: AssetItem
    associatedtype SessionType: AssetSession
    var sessions: [SessionType] {
        get
    }
    
}


public protocol AssetItem: class {
    associatedtype ContentType
    associatedtype ContextType
    
    init(asset: ContentType, context: ContextType)
    
}

public protocol AssetSession: class {
    associatedtype ItemType: AssetItem
    associatedtype Identifier: Hashable

    var identifier: Identifier {
        get
    }

    var title: String? {
        get
    }
    var lastAsset: ItemType? {
        get
    }
    var count: Int {
        get
    }
}

//public final class AssetItem<ContentType, ContextType> {
//    associatedtype ItemContent
//    associatedtype ItemContext
//
//    public let asset: ContentType
//    public var context: ContextType
//    public var identifier: String {
//        return ""
////        return self.asset.localIdentifier
//    }
//    public init(asset: ContentType, context: ContextType) {
//        self.asset = asset
//        self.context = context
//    }
//}


public protocol AssetManager {
    associatedtype ItemType: AssetItem

    func cachedThumbnailImage(_ identfier: ThumbnailIdentifier) -> UIImage?
    @discardableResult func requestThumbnailImage(asset: ItemType, options: ThumbnailOptions, resultHandler: @escaping (_ result: ImageRequestResult) -> Void) -> Request
}

open class AssetProvider<DataSource, Manager, Item>
where DataSource: AssetDataSource, DataSource.ItemType == Item,
Manager: AssetManager, Manager.ItemType == Item {
    
    public typealias Provider = AssetProvider<DataSource, Manager, Item>
    public typealias Session = DataSource.SessionType
    public enum SessionArrayChange {
        case remove([(Int, Session)])
        case insert([(Int, Session)])
    }
    public typealias GroupArrayObserverClosure = (_ entity: Provider, _ changes: [SessionArrayChange]) -> Void

    private var groupArrayObservers: [AnyHashable: GroupArrayObserverClosure] = [:]
    public var mode: PhotoMode {
        return self.option.mode
    }
    public var identifier: String {
        return self.option.identifier
    }
    public private(set) var sessionArray: [Session] = []
    public private(set) var sessionDictionary: [Session.Identifier: Session] = [:]
    
    public let option: ProviderOptions
    public let dataSource: DataSource
    public let manager: Manager
    
    public init(option: ProviderOptions, dataSource: DataSource, manager: Manager) {
        self.option = option
        self.dataSource = dataSource
        self.manager = manager
        self._loadAllGroups()
    }
    
    public func observeGroupArray(didChange: @escaping GroupArrayObserverClosure, forKey: AnyHashable) {
        self.groupArrayObservers[forKey] = didChange
    }
    public func unobserveGroupArray(forKey: AnyHashable) {
        self.groupArrayObservers.removeValue(forKey: forKey)
    }
    
//    private func _reloadAllGroups() -> [GroupArrayChange] {
//        let collections = AssetCollectionProvider.fetchAssetCollections(mode: self.option.mode)
//        var identifiers: Set<String> = []
//        for group in self.groupArray {
//            identifiers.insert(group.identifier)
//        }
//        var removed: [(Int, Group)] = []
//        var inserted: [(Int, Group)] = []
//        var groups: [Group] = []
//        var results: [Group] = []
//
//        for collection in collections {
//            let key = collection.localIdentifier
//            if identifiers.contains(key) {
//                identifiers.remove(key)
//            } else {
//                let group = AssetGroup(option: self.option, builder: self.builder, manager: self.manager, collection: collection)
//                self.groupDictionary[key] = group
//                groups.append(group)
//            }
//        }
//        for (index, group) in self.groupArray.enumerated() {
//            let key = group.identifier
//            if identifiers.contains(key) {
//                removed.append((index, group))
//            } else {
//                results.append(group)
//            }
//        }
//        for group in groups {
//            inserted.append((results.count, group))
//            results.append(group)
//        }
//        self.groupArray = results
//        var changes: [GroupArrayChange] = []
//        if !removed.isEmpty {
//            changes.append(.remove(removed))
//        }
//        if !inserted.isEmpty {
//            changes.append(.insert(inserted))
//        }
//        return changes
//    }
//
    private func _loadAllGroups() {
        let sessions = self.dataSource.sessions
        for session in sessions {
            let key = session.identifier
            if nil == self.sessionDictionary[key] {
                self.sessionArray.append(session)
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
}

extension ProviderContext: AssetBuilder {
    public typealias ItemContext = AssetContext
    public func item(asset: PHAsset) -> AssetItem<AssetContext> {
        if let result = self.dictionary[asset.localIdentifier] {
            return result
        } else {
            return AssetItem<AssetContext>.init(asset: asset, context: ItemContext())
        }
    }
}

//public final class AssetItem<ContextType> {
//    public let asset: PHAsset
//    public var context: ContextType
//    public var identifier: String {
//        return self.asset.localIdentifier
//    }
//    public init(asset: PHAsset, context: ContextType) {
//        self.asset = asset
//        self.context = context
//    }
//}

//public final class AssetGroup<BuilderType, ManagerType, ItemContext> where BuilderType: AssetBuilder, BuilderType.ItemContext == ItemContext {
//    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
//
//    public let identifier: String
//    private let collection: PHAssetCollection
//    public dynamic private(set) var title: String?
//    public dynamic private(set) var lastAsset: AssetItem<ItemContext>?
//    public dynamic private(set) var count: Int = 0
//    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
//    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]
//
//    public let option: ProviderOptions
//    public let builder: BuilderType
//    public let manager: ManagerType
//    private var thumbnailImageRequestId: PHImageRequestID?
//    public init(option: ProviderOptions, builder: BuilderType, manager: ManagerType, collection: PHAssetCollection) {
//        self.option = option
//        self.builder = builder
//        self.manager = manager
//        self.collection = collection
//        self.identifier = collection.localIdentifier
//        let options: PHFetchOptions = PHFetchOptions()
//        switch option.mode {
//        case .image:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue)", argumentArray: nil)
//        case .video:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
//        case .imageAndVideo:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue) || mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
//        }
//
//        let assetFetchResult: PHFetchResult<PHAsset> = PHAsset.fetchAssets(in: self.collection, options: options)
//        self.assetFetchResult = assetFetchResult
//        self.count = assetFetchResult.count
//        self.title = collection.localizedTitle
//        if let asset = self.assetFetchResult.lastObject {
//            self.lastAsset = self.builder.item(asset: asset)
//        } else {
//            self.lastAsset = nil
//        }
//    }
//
//    public func addDataObserver(_ observer: @escaping ObserverClosure, forKey: AnyHashable) {
//        self.observers[forKey] = observer
//    }
//    public func removeDataObserver(forKey: AnyHashable) {
//        self.observers.removeValue(forKey: forKey)
//    }
//
////    public var debugDescription: String {
////        get {
////            let name: String
////            if let localizedTitle = self.collection.localizedTitle {
////                name = localizedTitle
////            } else {
////                name = "nil"
////            }
////            let dateString: String
////            if let startDate = self.collection.startDate {
////                dateString = "\(startDate)"
////            } else {
////                dateString = "nil"
////            }
////            return "<Group: \(String(format: "%p", self)), name: \(name), startDate:\(dateString), count:\(self.count)>"
////        }
////    }
//
//    public func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
//        if let changeDetails: PHFetchResultChangeDetails<PHAsset> = changeInstance.changeDetails(for: self.assetFetchResult) {
//            self.assetFetchResult = changeDetails.fetchResultAfterChanges
//            self.count = self.assetFetchResult.count
//
//            let observers = self.observers
//            _ = observers.map { (item) -> Void in
//                let closure: ObserverClosure = item.value
//                closure(self, changeDetails)
//            }
//            return true
//        } else {
//            return false
//        }
//    }
//}


//open class AssetProvider<DataSource, Manager, Item>
//where DataSource: AssetDataSource, DataSource.ItemType == Item,
//Manager: AssetManager, Manager.ItemType == Item {
//
//    public typealias Provider = AssetProvider<DataSource, Manager, Item>
//    public typealias Session = DataSource.SessionType
//    public enum SessionArrayChange {
//        case remove([(Int, Session)])
//        case insert([(Int, Session)])
//    }
//    public typealias GroupArrayObserverClosure = (_ entity: Provider, _ changes: [SessionArrayChange]) -> Void
//
//    public let collectionProvider: AssetChangeObserver
//    private var groupArrayObservers: [AnyHashable: GroupArrayObserverClosure] = [:]
//    public var mode: PhotoMode {
//        return self.option.mode
//    }
//    public var identifier: String {
//        return self.option.identifier
//    }
//    public private(set) var groupArray: [Session] = []
//    public private(set) var groupDictionary: [String: Session] = [:]
//
//    public let option: ProviderOptions
//    public let builder: Builder
//    public let manager: Manager
//
//    public init(option: ProviderOptions, dataSource: DataSource, manager: Manager) {
//        self.option = option
//        self.builder = builder
//        self.manager = manager
//        self.collectionProvider = AssetChangeObserver(queue: DispatchQueue.main)
//        self._loadAllGroups()
//        self.collectionProvider.changeHandler = {[weak self] (changeInstance) -> Void in
//            guard let `self` = self else {
//                return
//            }
//            var changes = self._reloadAllGroups()
//            for group in self.groupArray {
//                group.handlePhotoLibraryChange(changeInstance)
//            }
//
//            if !changes.isEmpty {
//                let observers = self.groupArrayObservers
//                for (_, body) in observers {
//                    body(self, changes)
//                }
//            }
//        }
//    }
//
//    public func observeGroupArray(didChange: @escaping GroupArrayObserverClosure, forKey: AnyHashable) {
//        self.groupArrayObservers[forKey] = didChange
//    }
//    public func unobserveGroupArray(forKey: AnyHashable) {
//        self.groupArrayObservers.removeValue(forKey: forKey)
//    }
//
//    private func _reloadAllGroups() -> [GroupArrayChange] {
//        let collections = AssetCollectionProvider.fetchAssetCollections(mode: self.option.mode)
//        var identifiers: Set<String> = []
//        for group in self.groupArray {
//            identifiers.insert(group.identifier)
//        }
//        var removed: [(Int, Group)] = []
//        var inserted: [(Int, Group)] = []
//        var groups: [Group] = []
//        var results: [Group] = []
//
//        for collection in collections {
//            let key = collection.localIdentifier
//            if identifiers.contains(key) {
//                identifiers.remove(key)
//            } else {
//                let group = AssetGroup(option: self.option, builder: self.builder, manager: self.manager, collection: collection)
//                self.groupDictionary[key] = group
//                groups.append(group)
//            }
//        }
//        for (index, group) in self.groupArray.enumerated() {
//            let key = group.identifier
//            if identifiers.contains(key) {
//                removed.append((index, group))
//            } else {
//                results.append(group)
//            }
//        }
//        for group in groups {
//            inserted.append((results.count, group))
//            results.append(group)
//        }
//        self.groupArray = results
//        var changes: [GroupArrayChange] = []
//        if !removed.isEmpty {
//            changes.append(.remove(removed))
//        }
//        if !inserted.isEmpty {
//            changes.append(.insert(inserted))
//        }
//        return changes
//    }
//
//    private func _loadAllGroups() {
//        let collections = AssetCollectionProvider.fetchAssetCollections(mode: self.option.mode)
//        for collection in collections {
//            let key = collection.localIdentifier
//            if nil == self.groupDictionary[key] {
//                let group = AssetGroup(option: self.option, builder: self.builder, manager: self.manager, collection: collection)
//                self.groupDictionary[key] = group
//                self.groupArray.append(group)
//            }
//        }
//    }
//}
//
//public final class AssetContext: NSObject {
//    public dynamic var isSelected: Bool = false
//    public var cachePath: String? = nil
//    public override init() {
//
//    }
//}
//
//
//
//public final class ProviderContext {
//    public private(set) var dictionary: [String: AssetItem<AssetContext>] = [:]
//    public init() {
//
//    }
//
////    public func asset(asset: PHAsset) -> AssetItem<>
//}
//
//extension ProviderContext: AssetBuilder {
//    public typealias ItemContext = AssetContext
//    public func item(asset: PHAsset) -> AssetItem<AssetContext> {
//        if let result = self.dictionary[asset.localIdentifier] {
//            return result
//        } else {
//            return AssetItem<AssetContext>.init(asset: asset, context: ItemContext())
//        }
//    }
//}
//
////public final class AssetItem<ContextType> {
////    public let asset: PHAsset
////    public var context: ContextType
////    public var identifier: String {
////        return self.asset.localIdentifier
////    }
////    public init(asset: PHAsset, context: ContextType) {
////        self.asset = asset
////        self.context = context
////    }
////}
//
//public final class AssetGroup<BuilderType, ManagerType, ItemContext> where BuilderType: AssetBuilder, BuilderType.ItemContext == ItemContext {
//    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
//
//    public let identifier: String
//    private let collection: PHAssetCollection
//    public dynamic private(set) var title: String?
//    public dynamic private(set) var lastAsset: AssetItem<ItemContext>?
//    public dynamic private(set) var count: Int = 0
//    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
//    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]
//
//    public let option: ProviderOptions
//    public let builder: BuilderType
//    public let manager: ManagerType
//    private var thumbnailImageRequestId: PHImageRequestID?
//    public init(option: ProviderOptions, builder: BuilderType, manager: ManagerType, collection: PHAssetCollection) {
//        self.option = option
//        self.builder = builder
//        self.manager = manager
//        self.collection = collection
//        self.identifier = collection.localIdentifier
//        let options: PHFetchOptions = PHFetchOptions()
//        switch option.mode {
//        case .image:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue)", argumentArray: nil)
//        case .video:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
//        case .imageAndVideo:
//            options.predicate = NSPredicate(format: "mediaType == \(PHAssetMediaType.image.rawValue) || mediaType == \(PHAssetMediaType.video.rawValue)", argumentArray: nil)
//        }
//
//        let assetFetchResult: PHFetchResult<PHAsset> = PHAsset.fetchAssets(in: self.collection, options: options)
//        self.assetFetchResult = assetFetchResult
//        self.count = assetFetchResult.count
//        self.title = collection.localizedTitle
//        if let asset = self.assetFetchResult.lastObject {
//            self.lastAsset = self.builder.item(asset: asset)
//        } else {
//            self.lastAsset = nil
//        }
//    }
//
//    public func addDataObserver(_ observer: @escaping ObserverClosure, forKey: AnyHashable) {
//        self.observers[forKey] = observer
//    }
//    public func removeDataObserver(forKey: AnyHashable) {
//        self.observers.removeValue(forKey: forKey)
//    }
//
////    public var debugDescription: String {
////        get {
////            let name: String
////            if let localizedTitle = self.collection.localizedTitle {
////                name = localizedTitle
////            } else {
////                name = "nil"
////            }
////            let dateString: String
////            if let startDate = self.collection.startDate {
////                dateString = "\(startDate)"
////            } else {
////                dateString = "nil"
////            }
////            return "<Group: \(String(format: "%p", self)), name: \(name), startDate:\(dateString), count:\(self.count)>"
////        }
////    }
//
//    public func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
//        if let changeDetails: PHFetchResultChangeDetails<PHAsset> = changeInstance.changeDetails(for: self.assetFetchResult) {
//            self.assetFetchResult = changeDetails.fetchResultAfterChanges
//            self.count = self.assetFetchResult.count
//
//            let observers = self.observers
//            _ = observers.map { (item) -> Void in
//                let closure: ObserverClosure = item.value
//                closure(self, changeDetails)
//            }
//            return true
//        } else {
//            return false
//        }
//    }
//
//}
