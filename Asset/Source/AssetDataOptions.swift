//
//  AssetDataOptions.swift
//  Asset
//
//  Created by vector on 2019/10/30.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public enum PhotoMode {
    case image
    case video
    case imageAndVideo
}

public final class AssetDataOptions {
    //Thumbnail
    public let identifier: String
    public let mode: PhotoMode

    public init(mode: PhotoMode = .imageAndVideo) {
        self.mode = mode
        self.identifier = UUID().uuidString
    }
    
//        lazy var cachingImageManager: PHCachingImageManager = {
//            return PHCachingImageManager()
//        }()
//        @discardableResult public func requestThumbnailImage(asset: PHAsset, size: CGSize, resultHandler: @escaping (_ requestId: PHImageRequestID, _ image: UIImage?, _ error: NSError?) -> Void) -> PHImageRequestID? {
//            let options = PHImageRequestOptions()
//            options.isNetworkAccessAllowed = true
//            options.resizeMode = .exact
//            options.isSynchronous = true
//            let id = self.cachingImageManager.requestImage(for: asset, targetSize: size, contentMode: .aspectFill, options: options) { (image, info) in
//                DispatchQueue.main.async {
//                    var rid = PHInvalidImageRequestID
//                    if let number = info?[PHImageResultRequestIDKey] as? NSNumber {
//                        rid = PHImageRequestID(truncating: number)
//                    }
//
//                    if let error = info?[PHImageErrorKey] as? NSError {
//                        resultHandler(rid, image, error)
//                    } else {
//                        resultHandler(rid, image, nil)
//                    }
//                }
//            }
//            if id == PHInvalidImageRequestID {
//                return nil
//            } else {
//                return id
//            }
//        }
}
