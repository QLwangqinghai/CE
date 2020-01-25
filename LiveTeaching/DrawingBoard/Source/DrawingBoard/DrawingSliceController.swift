//
//  DrawingSliceController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class SliceContent: NSObject {
    public let info: SliceInfo
    public let content: ByteBuffer
    public init(info: SliceInfo, content: ByteBuffer) {
        self.info = info
        self.content = content
    }
    deinit {
    }
}

public class SliceContentProvider: SliceContent {

}


public protocol DrawingContentDataSource: class {
    func prepare(slices: [Slice])
    func suspend()
    
    func beginAccess(slice: Slice) -> SliceContent
    func endAccess(slice: Slice, context: SliceContent)
    func didSliceContextUpdate(slice: Slice, context: SliceContent)
}


public class DrawingBoardProvider: DrawingContentDataSource {
    public let bitmapLayout: BitmapLayout
    public init(bitmapLayout: BitmapLayout) {
        self.bitmapLayout = bitmapLayout
    }
    
    private var map: [String: ByteBuffer] = [:]
    public func prepare(slices: [Slice]) {
        for slice in slices {
            assert(self.map[slice.info.identifier] == nil)
        }
    }
    public func suspend() {
        
    }

    public func beginAccess(slice: Slice) -> SliceContent {
        let buffer: ByteBuffer
        if let v = self.map[slice.info.identifier] {
            buffer = v
        } else {
            buffer = ByteBuffer(size: slice.info.bitmapLayout.bytesPerRow * Int(slice.info.y))
            self.map[slice.info.identifier] = buffer
        }
        return SliceContent(info: slice.info, content: buffer)
    }
    public func endAccess(slice: Slice, context: SliceContent) {

    }
    public func didSliceContextUpdate(slice: Slice, context: SliceContent) {
        
    }
}


public class DrawingBoardFileDataProvider: DrawingContentDataSource {
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
    public func suspend() {
        
    }
    public func beginAccess(slice: Slice) -> SliceContent {
        let buffer = ByteBuffer(size: 1)
        return SliceContent(info: slice.info, content: buffer)
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

public class DrawingSliceController<T: NSObject>: BaseController<T> {
    public var identifier: String

    public private(set) var sequence: UInt64 = 0

    public let pageContext: DrawingPageContext
    
    public let slices: [Slice]
    public let sliceHeight: UInt32
    public private(set) var visibleSlices: [Slice]
    public let layer: CALayer
    public let dataSource: DrawingContentDataSource
    public let domain: String
    public let observer: DrawingPageContext.Observer
    
    public init(pageContext: DrawingPageContext, domain: String, dataSource: DrawingContentDataSource) {
        self.pageContext = pageContext
        let identifier = pageContext.identifier.appending("/\(domain)")
        self.identifier = identifier
        self.domain = domain
        self.observer = DrawingPageContext.Observer(identifier: identifier)
        let layer: CALayer = CALayer()
        layer.frame = pageContext.bounds
        layer.masksToBounds = true
        self.dataSource = dataSource
        let sliceHeight: UInt32 = 256
        let contentSize = pageContext.contentSize
        let sliceCount: UInt32 = (contentSize.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: contentSize.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                slices.append(Slice(ownerIdentifier: identifier, y: y, size: sliceSize, bitmapLayout: pageContext.bitmapLayout))
            }
        }
        self.slices = slices
        self.sliceHeight = sliceHeight
        self.visibleSlices = []
        self.layer = layer
        super.init()
    }
    
}

