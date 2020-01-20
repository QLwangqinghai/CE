//
//  File.swift
//  File
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

public struct File {
    public struct Size {
        public var width: UInt32
        public var height: UInt32
    }
    
    public struct Image {
        public let encode: Int32
        public let size: Size
    }
    public struct Video {
//        public let encode: Int32
        public let size: Size
        public let duration: Int64
    }
    public enum FileType {
        case image(Image)
        case video(Video)
    }
    
    public class Meta {
        public let type: FileType
        public let id: String
        public let size: UInt64
        
        
    }
    public class Inode {
        
        
        
        
    }
    
}
