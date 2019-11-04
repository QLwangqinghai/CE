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

class RequestHandlerDictionary<Key, Value> where Key: Hashable, Value: Request {
    private class HandlerArray {
        var array: [Value] = []
        init() {}
        func append(_ item: Value) {
            self.array.append(item)
        }
    }
    private let lock = NSLock()
    private var dictionary: [Key: HandlerArray] = [:]

    
    public init() {}
    
    //第一次插入 返回true
    func append(_ value: Value, forKey key: Key) -> Bool {
        var result: Bool = false
        self.lock.lock()
        if let array = self.dictionary[key] {
            array.append(value)
        } else {
            let array = HandlerArray()
            array.append(value)
            self.dictionary[key] = array
            result = true
        }
        self.lock.unlock()
        return result
    }
    
    func contain(key: Key) -> Bool {
        var result: Bool = false
        self.lock.lock()
        if let list = self.dictionary[key] {
            list.array = list.array.filter({ (item) -> Bool in
                return !item.isCanceled && !item.isFinished
            })
            if list.array.isEmpty {
                self.dictionary[key] = nil
            } else {
                result = true
            }
        }
        self.lock.unlock()
        return result
    }
    
    func remove(forKey key: Key) -> [Value]? {
        var result: [Value]?
        self.lock.lock()
        if let array = self.dictionary[key] {
            result = array.array
            self.dictionary[key] = nil
        }
        self.lock.unlock()
        return result
    }
}

//open class AssetProvider<BuilderType, ManagerType, ItemContext> where BuilderType: AssetItemBuilder, BuilderType.ItemContext == ItemContext, ManagerType: AssetManager, ManagerType.ItemContext == ItemContext {

extension DispatchQueue {
    internal static let assetQueue: DispatchQueue = DispatchQueue(label: "asset")
}

public class CachingImageManager {
    private class Handler: Request {
        static let lock: NSLock = NSLock()
        
        static func execute<T>(handler: Handler, body: () -> T) -> T {
            self.lock.lock()
            let result = body()
            self.lock.unlock()
            return result
        }
        
        enum Status {
            case cancel
            case finish
            case wait((_ result: ImageRequestResult) -> Void)
        }
        
        var isCanceled: Bool {
            switch self.status {
            case .cancel:
                return true
            default:
                return false
            }
        }
        
        var isFinished: Bool {
            switch self.status {
            case .finish:
                return true
            default:
                return false
            }
        }
        func finish(result: ImageRequestResult) {
            if let handler = Handler.execute(handler: self, body:{ () -> ((_ result: ImageRequestResult) -> Void)? in
                if case let .wait(body) = self.status {
                    self.status = .finish
                    return body
                } else {
                    return nil
                }
            }) {
                handler(result)
            }
        }
        func cancel() {
            if let handler = Handler.execute(handler: self, body:{ () -> ((_ result: ImageRequestResult) -> Void)? in
                if case let .wait(body) = self.status {
                    self.status = .cancel
                    return body
                } else {
                    return nil
                }
            }) {
                handler(.cancel)
            }
        }
        var status: Status
        init(handler: @escaping (_ result: ImageRequestResult) -> Void) {
            self.status = .wait(handler)
        }
    }
    private class HandlerArray {
        var array: [Handler] = []
        init() {}
        
        func append(_ handler: Handler) {
            self.array.append(handler)
        }
    }

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
        
        public func image(forKey key: ThumbnailIdentifier) -> UIImage? {
            return self.cache.object(forKey: key)
        }
        public func cache(image: UIImage, forKey key: ThumbnailIdentifier) {
            var cost: Int = Int(image.size.width * image.size.height * image.scale)
            if let images = image.images {
                cost = cost * images.count
            }
            self.cache.setObject(image, forKey: key, cost: cost)
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
    
    private var thumbnailImageHandlers: RequestHandlerDictionary<ThumbnailIdentifier, Handler> = RequestHandlerDictionary<ThumbnailIdentifier, Handler>()
    
    public init(totalPixelLimit: Int = CachingImageManager.defaultTotalPixelLimit) {
        self.cache = Cache(totalPixelLimit: totalPixelLimit)
    }
        
    public func image(forKey key: ThumbnailIdentifier) -> UIImage? {
        return self.cache.cache.object(forKey: key)
    }
    
//    let identifier = ThumbnailIdentifier(identifier: asset.localIdentifier, options: option)

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
    
    enum RequestImageResult {
        case success(UIImage)
        case error(NSError)
    }
    
    func requestImage(asset: PHAsset, options: ThumbnailOptions, resultHandler: @escaping (_ result: RequestResult<UIImage, NSError?>) -> Void) -> PHImageRequestID {
        return self.cachingImageManager.requestImage(for: asset, targetSize: CGSize(width: options.width, height: options.height), contentMode: options.contentMode, options: CachingImageManager.thumbnailImageRequestOptions) { (image, info) in
            if let isCanceled = info?[PHImageCancelledKey] as? NSNumber {
                if isCanceled.boolValue {
                    resultHandler(.cancel)
                    return
                }
            }
            if let image = image {
                resultHandler(.success(image))
            } else {
                if let error = info?[PHImageErrorKey] as? NSError {
                    resultHandler(.error(error))
                } else {
                    resultHandler(.error(nil))
                }
            }
        }
//        if id == PHInvalidImageRequestID {
//            return nil
//        } else {
//            return id
//        }
    }
    
//    public func requestThumbnailImage(asset: PHAsset, options: ThumbnailOptions, resultHandler: @escaping (_ image: UIImage?, _ error: NSError?) -> Void) {
//        let id = self.cachingImageManager.requestImage(for: asset, targetSize: CGSize(width: options.width, height: options.height), contentMode: options.contentMode, options: CachingImageManager.thumbnailImageRequestOptions) { (image, info) in
//            DispatchQueue.main.async {
//                var rid = PHInvalidImageRequestID
//                if let number = info?[PHImageResultRequestIDKey] as? NSNumber {
//                    rid = PHImageRequestID(truncating: number)
//                }
//                if let error = info?[PHImageErrorKey] as? NSError {
//                    resultHandler(rid, image, error)
//                } else {
//                    resultHandler(rid, image, nil)
//                }
//            }
//        }
//        if id == PHInvalidImageRequestID {
//            return nil
//        } else {
//            return id
//        }
//    }
    
    func diskImage(identifier: ThumbnailIdentifier) -> UIImage? {
        
        return nil
    }
    
    func finishRequestThumbnailImage(identifier: ThumbnailIdentifier, result: ImageRequestResult) {
        DispatchQueue.main.async {
            if let array = self.thumbnailImageHandlers.remove(forKey: identifier) {
                for item in array {
                    item.finish(result: result)
                }
            }
        }
    }
    
    func _requestThumbnailImage(asset: AssetItem<ItemContext>, identifier: ThumbnailIdentifier) {
        if let image = self.cache.image(forKey: identifier) {
            //memory cache
            self.finishRequestThumbnailImage(identifier: identifier, result: .success(image))
        } else if let image = self.diskImage(identifier: identifier) {
            //file cache
            self.finishRequestThumbnailImage(identifier: identifier, result: .success(image))
        } else {
            //request
//            self.finishRequestThumbnailImage(result: .success(image))

            
        }
    }
    
    
}

extension CachingImageManager: AssetManager {
    public typealias ItemContext = AssetContext

    public func cachedThumbnailImage(_ identfier: ThumbnailIdentifier) -> UIImage? {
        return self.cache.image(forKey: identfier)
    }
    
    @discardableResult public func requestThumbnailImage(asset: AssetItem<ItemContext>, options: ThumbnailOptions, resultHandler: @escaping (_ result: ImageRequestResult) -> Void) -> Request {
        let identifier = ThumbnailIdentifier(identifier: asset.identifier, options: options)
        let request = Handler(handler: resultHandler)
        if self.thumbnailImageHandlers.append(request, forKey: identifier) {
            //首次请求
            DispatchQueue.assetQueue.async {
                self._requestThumbnailImage(asset: asset, identifier: identifier)
            }
        }
        return request
    }
    
}
