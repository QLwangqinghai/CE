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

//public enum FileType {
//    case archive
//    case image(type: ImageType)
//    case video
//    case audio
//    case text
//}
public enum FileType {
//    case archive
    case image(ImageAsset)
    case av(AVAsset)
    case text
}

//文件原始内容，不包含名称等
public struct FileMeta {
    public var id: CCByte64
    public var type: FileType
    public var size: UInt64
    public init(id: CCByte64, type: FileType, size: UInt64) {
        self.id = id
        self.type = type
        self.size = size
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
