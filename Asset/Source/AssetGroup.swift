//
//  AssetGroup.swift
//  Asset
//
//  Created by vector on 2019/10/30.
//  Copyright © 2019 angfung. All rights reserved.
//

import Photos

public final class AssetGroup: NSObject, ObservableObject {
    public typealias ObserverClosure = (_ group: AssetGroup, _ changeDetails: PHFetchResultChangeDetails<PHAsset>) -> Void
    
    public let identifier: String

    private let collection: PHAssetCollection
    public dynamic private(set) var title: String?
    public dynamic private(set) var lastAsset: PHAsset?
    public dynamic private(set) var count: Int = 0
    public private(set) var assetFetchResult: PHFetchResult<PHAsset>
    public private(set) var observers: [AnyHashable: ObserverClosure] = [:]
    
    public let option: AssetDataOptions
    private var thumbnailImageRequestId: PHImageRequestID?
    public init(context: AssetDataOptions, collection: PHAssetCollection) {
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

    @discardableResult public func handlePhotoLibraryChange(_ changeInstance: PHChange) -> Bool {
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
