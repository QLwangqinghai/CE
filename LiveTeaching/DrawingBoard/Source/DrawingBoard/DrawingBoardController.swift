//
//  DrawingBoardController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class SliceContent: NSObject {
    public let info: SliceInfo
    public let content: UnsafeMutableRawPointer
    public init(info: SliceInfo, content: UnsafeMutableRawPointer) {
        self.info = info
        self.content = content
    }
    deinit {
    }
}

public class SliceContentProvider: SliceContent {

}



public protocol DrawingBoardDataSource: class {
    func prepare(slices: [Slice])
    func beginAccess(slice: Slice) -> SliceContent
    func endAccess(slice: Slice, context: SliceContent)
    func didSliceContextUpdate(slice: Slice, context: SliceContent)
}

public class DrawingBoardProvider: DrawingBoardDataSource {
    public let bitmapLayout: BitmapLayout
    public init(bitmapLayout: BitmapLayout) {
        self.bitmapLayout = bitmapLayout
    }
    
    private var fileDatas: [String: FileData] = [:]
    public func prepare(slices: [Slice]) {
        for slice in slices {
            assert(self.fileDatas[slice.info.identifier] == nil)
            let fileData = FileManager.shared.beginAccessFileData(path: slice.info.identifier)
            self.fileDatas[slice.info.identifier] = fileData
        }
    }

    public func beginAccess(slice: Slice) -> SliceContent {
        return SliceContent.init(info: slice.info, content: UnsafeMutableRawPointer.allocate(byteCount: 10, alignment: 1))
    }
    public func endAccess(slice: Slice, context: SliceContent) {

    }
    public func didSliceContextUpdate(slice: Slice, context: SliceContent) {
        
    }
    deinit {
        for (_, fileData) in self.fileDatas {
            FileManager.shared.endAccess(fileData: fileData)
        }
    }
    
}

//public class SliceContext: NSObject {
//    public let info: SliceInfo
//    public let fileContent: FileContent
//    public let fileData: FileData
//
//    public init(info: SliceInfo, fileData: FileData) {
////        let filehandle: FileHandle = FileHandle.init(forReadingAtPath: "")!
////        let data = filehandle.readData(ofLength: 9)
////        data.withUnsafeBytes { (ptr) -> Void in
////            CCMemoryCopy(self.ptr, ptr, )
////        }
//        self.info = info
//        self.fileData = fileData
//        self.fileContent = fileData.beginContentAccess()
//    }
//    deinit {
//        self.fileData.endContentAccess()
//    }
//}

public class SliceInfo: NSObject {
    public let identifier: String
    public let ownerIdentifier: String
    public let y: Int32
    public let size: Size
    public let bitmapLayout: BitmapLayout
    public init(ownerIdentifier: String, y: Int32, size: Size, bitmapLayout: BitmapLayout) {
        self.ownerIdentifier = ownerIdentifier
        self.y = y
        self.size = size
        self.bitmapLayout = bitmapLayout
        self.identifier = ownerIdentifier.appending("/\(y)")
    }
}
public class Slice: NSObject {
    public let info: SliceInfo
    public private(set) var content: SliceContent? = nil
    public let layer: CALayer
    
    public init(ownerIdentifier: String, y: Int32, size: Size, bitmapLayout: BitmapLayout) {
        self.info = SliceInfo(ownerIdentifier: ownerIdentifier, y: y, size: size, bitmapLayout: bitmapLayout)
        self.layer = CALayer()
    }

    public init(info: SliceInfo) {
        self.info = info
        self.layer = CALayer()
    }
    
}





public class DrawingBoardController {
    public static let zPosition: CGFloat = 100

    public let identifier: String
    public private(set) var sequence: UInt64 = 0

    private let items: [Slice]
    public let size: Size
    public let sliceHeight: UInt32

    public private(set) var offset: Int32
    public private(set) var visibleSlices: [Slice]

    public let bitmapLayout: BitmapLayout
    
    public let layer: CALayer
    public let scale: CGFloat
    
    public init(identifier: String, size: Size, offset: Int32, bitmapLayout: BitmapLayout, scale: CGFloat, layer: CALayer) {
        let sliceHeight: UInt32 = 256
        let sliceCount: UInt32 = (size.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: size.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                let slice = Slice(ownerIdentifier: identifier, y: y, size: sliceSize, bitmapLayout: bitmapLayout)
                slice.layer.zPosition = DrawingBoardController.zPosition
                slice.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: CGFloat(size.width) / scale, height: CGFloat(sliceHeight) / scale)
                layer.addSublayer(slice.layer)
                slices.append(slice)
            }
        }
        self.items = slices
        self.sliceHeight = sliceHeight
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.size = size
        self.offset = offset
        self.visibleSlices = []
        self.layer = layer
        self.scale = scale
    }
    
    public func beginDrawing(borad: DrawingBoard) throws {
        try borad.begin(controller: self, offset: self.offset)
    }
    
    public func pointBuffer(y: Int32) -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer.allocate(byteCount: 10, alignment: CCGetCachelineSize())
    }
}

