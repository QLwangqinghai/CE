//
//  FileItem.swift
//  FileManager
//
//  Created by vector on 2019/7/18.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public typealias PixelSize = CFMPixelSize_s;
public typealias FileIdentifier = CFMByte64_s;

public enum ImageType {
    case jpg
    case png
    case gif
    case heic
    case webp
}

public enum FileType {
    case archive
    case image(type: ImageType)
    case video
    case audio
    case text
    
}

public class ImageItem {
    public let size: PixelSize
    public let imageType: ImageType
    public let relativePath: String
    
    public init(size: PixelSize, imageType: ImageType, relativePath: String) {
        self.size = size
        self.imageType = imageType
        self.relativePath = relativePath
    }
}
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
//
//
//
//public class FileItem {
//    public let id: FileIdentifier
//    public let name: String
//    public let fileSize: UInt64
//
//    public let fileType: FileType
//    public let relativePath: String
//    public let icon: ImageItem
//    public let previewImage: ImageItem
//
//    
//    
//}


//@property (nonatomic, copy) NSString * fileId;
//
//@property (nonatomic, copy) NSString * name;
//
//@property (nonatomic, copy) NSString * urlString;
//@property (nonatomic, copy) NSString * md5String;
//
//@property (nonatomic, assign) NSInteger fileSize;
//@property (nonatomic, assign) NSInteger type;//文件类型
//
//@property (nonatomic, assign) NSInteger width;
//@property (nonatomic, assign) NSInteger height;
//@property (nonatomic, assign) NSInteger duration;
//
//@property (nonatomic, strong) SCImageModel * previewImage;
//
//@property (nonatomic, readonly) BOOL isVideo;
//@property (nonatomic, readonly) BOOL isImage;
