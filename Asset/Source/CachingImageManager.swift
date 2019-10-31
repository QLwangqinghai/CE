//
//  CachingImageManager.swift
//  Asset
//
//  Created by vector on 2019/10/31.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

public struct ThumbnailOptions: Equatable {
    public let width: Int
    public let height: Int
    public let contentMode: PHImageContentMode

    public init(width: Int, height: Int, contentMode: PHImageContentMode) {
        self.width = width
        self.height = height
        self.contentMode = contentMode
    }
    public init(size: CGSize, contentMode: PHImageContentMode) {
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
    public let identifier: String
    public let options: ThumbnailOptions

    public init(identifier: String, options: ThumbnailOptions) {
        self.identifier = identifier
        self.options = options
    }

    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.identifier)
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

public class CachingImageManager {
    public class Cache {
        public let cache: MemoryCache<ThumbnailIdentifier, UIImage>
        private var memoryWarningObserver: NSObjectProtocol? = nil
        public init(totalPixelLimit: Int) {
            self.cache = MemoryCache<ThumbnailIdentifier, UIImage>()
            self.cache.totalCostLimit = totalPixelLimit
            self.memoryWarningObserver = NotificationCenter.default.addObserver(forName: UIApplication.didReceiveMemoryWarningNotification, object: nil, queue: nil) {[weak self] (_) in
                guard let `self` = self else {
                    return
                }
                self.didReceiveMemoryWarning()
            }
        }
        
        deinit {
            if let memoryWarningObserver = self.memoryWarningObserver {
                NotificationCenter.default.removeObserver(memoryWarningObserver)
            }
        }
        
        private func didReceiveMemoryWarning() {
            self.cache.removeAllObjectStrongRefrences()
        }
    }
    
    public let cache: Cache
    lazy var cachingImageManager: PHCachingImageManager = {
        return PHCachingImageManager()
    }()
    
    private static let thumbnailImageRequestOptions: PHImageRequestOptions = {
        let options = PHImageRequestOptions()
        options.isNetworkAccessAllowed = true
        options.resizeMode = .exact
        options.isSynchronous = true
        return options
    }()
    
    public static let defaultTotalPixelLimit: Int = {
        let scale = UIScreen.main.scale
        let size = UIScreen.main.bounds.size
        let pixelCount = Int(size.width * scale * size.height * scale + 0.5)
        return pixelCount
    }()
    
    public init(totalPixelLimit: Int = CachingImageManager.defaultTotalPixelLimit) {
        self.cache = Cache(totalPixelLimit: totalPixelLimit)
    }
    
    public func image(forKey key: ThumbnailIdentifier) -> UIImage? {
        return self.cache.cache.object(forKey: key)
    }
    
    @discardableResult public func requestThumbnailImage(asset: PHAsset, options: ThumbnailOptions, resultHandler: @escaping (_ requestId: PHImageRequestID, _ image: UIImage?, _ error: NSError?) -> Void) -> PHImageRequestID? {
        let id = self.cachingImageManager.requestImage(for: asset, targetSize: CGSize(width: options.width, height: options.height), contentMode: options.contentMode, options: CachingImageManager.thumbnailImageRequestOptions) { (image, info) in
            DispatchQueue.main.async {
                var rid = PHInvalidImageRequestID
                if let number = info?[PHImageResultRequestIDKey] as? NSNumber {
                    rid = PHImageRequestID(truncating: number)
                }
                if let error = info?[PHImageErrorKey] as? NSError {
                    resultHandler(rid, image, error)
                } else {
                    resultHandler(rid, image, nil)
                }
            }
        }
        if id == PHInvalidImageRequestID {
            return nil
        } else {
            return id
        }
    }
}

