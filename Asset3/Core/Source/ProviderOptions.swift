//
//  AssetDataOptions.swift
//  Asset
//
//  Created by vector on 2019/10/30.
//  Copyright © 2019 angfung. All rights reserved.
//

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
