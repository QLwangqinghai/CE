//
//  FileItem.swift
//  FileManager
//
//  Created by vector on 2019/7/18.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public class FileItem {
    public enum FileType {
        case archive
        case image
        case video
        case audio
        case text
        
    }
    
    
    typealias PixelSize = CFMPixelSize_s;
    typealias Identifier = CFMFileIdentifier_s;

    public let id: Identifier
    public let name: String
    public let fileSize: UInt64

    public let fileType: FileType
    
    @property (nonatomic, copy) NSString * fileId;
    
    @property (nonatomic, copy) NSString * name;
    
    @property (nonatomic, copy) NSString * urlString;
    @property (nonatomic, copy) NSString * md5String;
    
    @property (nonatomic, assign) NSInteger fileSize;
    @property (nonatomic, assign) NSInteger type;//文件类型
    
    @property (nonatomic, assign) NSInteger width;
    @property (nonatomic, assign) NSInteger height;
    @property (nonatomic, assign) NSInteger duration;
    
    @property (nonatomic, strong) SCImageModel * previewImage;
    
    @property (nonatomic, readonly) BOOL isVideo;
    @property (nonatomic, readonly) BOOL isImage;
    
    
}



