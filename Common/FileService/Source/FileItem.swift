//
//  FileItem.swift
//  FileManager
//
//  Created by vector on 2019/7/18.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation
@_exported import CCFoundation


public enum ImageFormat {
    case jpg
    case png
    case gif
    case heic
    case webp
    case unknown
}

public protocol Assetable {
    public var identifier: CCByte64

    
}

public class BaseAsset {
    public let format: ImageFormat
    public let size: CCSize32
    public let count: UInt32
    public init(format: ImageFormat, size: CCSize32, count: UInt32 = 1) {
        self.format = format
        self.size = size
        self.count = count
    }
}
public struct ImageAsset {
    public let format: ImageFormat
    public let size: CCSize32
    public let count: UInt32
    public init(format: ImageFormat, size: CCSize32, count: UInt32 = 1) {
        self.format = format
        self.size = size
        self.count = count
    }
}

public enum AVFormat {
    case mp4
}
public struct AVAsset {
    public let format: AVFormat
    public let size: CCSize32
    public let duration: Int64
    public init(format: AVFormat, size: CCSize32, duration: CCMicrosecondTime) {
        self.format = format
        self.size = size
        self.duration = duration
    }
}

public struct PDFAsset {
    public let size: CCSize32
    public let count: UInt32 //number of pages
    public init(size: CCSize32, count: UInt32) {
        self.size = size
        self.count = count
    }
}

//public enum FileType {
//    case archive
//    case image(type: ImageType)
//    case video
//    case audio
//    case text
//}

//pdf 可执行文件 图像 应用程序 归档 影片 文件夹 文本 文稿 演示文稿 音乐 其他

public enum FileType {
//    case archive
    case image(ImageAsset)
    case av(AVAsset)
    case text
}

//文件原始内容，不包含名称等
public struct FileMeta {
    public var id: FileIdentifier
    public var type: FileType
    public var size: UInt64 {
        return self.id.length
    }
    public init(id: FileIdentifier, type: FileType) {
        self.id = id
        self.type = type
    }
}

public class FileItem {
    public let id: UInt64
    public let meta: FileMeta
    public let name: String
    
    public init(id: UInt64, meta: FileMeta, name: String) {
        self.id = id
        self.meta = meta
        self.name = name
    }
}

//public enum FileType {
//    case archive
//    case image(type: ImageType)
//    case video
//    case audio
//    case text
//}

//
//open class FileDigestInfo {
//    //1/8192  多出来的
//    
//    public static let stepLength: UInt64 = 0x80000
//    public let fileSize: UInt64
//    public let final: CFMByte64_s
//    public let stepDigests: [CFMByte64_s]
//    
//    
//    
//    
//}
