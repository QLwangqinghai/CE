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
