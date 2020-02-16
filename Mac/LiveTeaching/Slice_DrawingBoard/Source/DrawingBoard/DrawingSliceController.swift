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
    func beginAccess(slices: [Slice])
    func endAccess(slices: [Slice])
    func didSliceContentUpdate(slices: [Slice])
}


public class DrawingBoardProvider: DrawingContentDataSource {
    public let bitmapLayout: BitmapLayout
    public init(bitmapLayout: BitmapLayout) {
        self.bitmapLayout = bitmapLayout
    }
    
    private var map: [String: SliceContent] = [:]
    public func prepare(slices: [Slice]) {
        for slice in slices {
            assert(self.map[slice.info.identifier] == nil)
        }
    }
    public func beginAccess(slices: [Slice]) {
        for slice in slices {
            if slice.content == nil {
                if let content = self.map[slice.info.identifier] {
                    slice.content = content
                } else {
                    let buffer = ByteBuffer(size: slice.info.bitmapLayout.bytesPerRow * Int(slice.info.y))
                    let content = SliceContent(info: slice.info, content: buffer)
                    self.map[slice.info.identifier] = content
                    slice.content = content
                }
            }
        }
    }
    public func endAccess(slices: [Slice]) {
        for slice in slices {
            slice.content = nil
        }
    }
    public func didSliceContentUpdate(slices: [Slice]) {
        
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
    public func beginAccess(slices: [Slice]) {

    }
    public func endAccess(slices: [Slice]) {

    }
    public func didSliceContentUpdate(slices: [Slice]) {
        
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
    public let index: Int
    public let size: Size
    public let bitmapLayout: BitmapLayout
    public init(ownerIdentifier: String, y: Int32, size: Size, bitmapLayout: BitmapLayout, index: Int) {
        self.ownerIdentifier = ownerIdentifier
        self.y = y
        self.size = size
        self.index = index
        self.bitmapLayout = bitmapLayout
        self.identifier = ownerIdentifier.appending("/\(y)")
    }
}
public class Slice: NSObject {
    public let info: SliceInfo
    private var needsDisplay: Bool = true
    public var content: SliceContent? = nil {
        didSet(old) {
            if old != self.content {
                self.setNeedsDisplay()
                self.displayIfNeeded()
            }
        }
    }
    public let layer: CALayer
    
    public init(ownerIdentifier: String, y: Int32, size: Size, bitmapLayout: BitmapLayout, index: Int) {
        self.info = SliceInfo(ownerIdentifier: ownerIdentifier, y: y, size: size, bitmapLayout: bitmapLayout, index: index)
        self.layer = CALayer()
    }

    public init(info: SliceInfo) {
        self.info = info
        self.layer = CALayer()
    }
    
    public func setNeedsDisplay() {
        self.needsDisplay = true
    }
    
    public func displayIfNeeded() {
        if self.needsDisplay {
            self.display()
        }
    }
    public func display() {
        
    }
}

public class DrawingSliceController<T: NSObject>: BaseController<T> {
    public var identifier: String

    public private(set) var sequence: UInt64 = 0

    public let pageContext: DrawingPageContext
    
    public let slices: [Slice]
    public let sliceHeight: UInt32
    public private(set) var visibleSlices: [Slice]
    public let dataSource: DrawingContentDataSource
    public let domain: String
    public let observer: DrawingPageContext.Observer
    
    public init(pageContext: DrawingPageContext, domain: String, dataSource: DrawingContentDataSource) {
        self.pageContext = pageContext
        let identifier = pageContext.identifier.appending("/\(domain)")
        self.identifier = identifier
        self.domain = domain
        let observer = DrawingPageContext.Observer(identifier: identifier)
        self.observer = observer
        self.dataSource = dataSource
        let sliceHeight: UInt32 = 64 * 3
        let contentSize = pageContext.contentSize
        let sliceCount: UInt32 = (contentSize.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: contentSize.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                slices.append(Slice(ownerIdentifier: identifier, y: y, size: sliceSize, bitmapLayout: pageContext.bitmapLayout, index: Int(index)))
            }
        }
        self.slices = slices
        self.sliceHeight = sliceHeight
        self.visibleSlices = []
        super.init()
    }
    
    public override func contentDidLoad(_ content: T) {
        super.contentDidLoad(content)
        self.observer.didUpdateOffset = {[weak self] (_ observer: DrawingPageContext.Observer, _ context: DrawingPageContext, _ from: Int32, _ to: Int32) in
            guard let `self` = self else {
                return
            }
            self.resetVisibleSlices()
        }
        
        self.observer.didUpdateIsHidden = {[weak self] (_ observer: DrawingPageContext.Observer, _ context: DrawingPageContext, _ to: Bool) in
            guard let `self` = self else {
                return
            }
            self.resetVisibleSlices()
        }
        self.pageContext.addObserver(self.observer)
    }
    
    private func resetVisibleSlices() {
        guard !self.slices.isEmpty else {
            return
        }
        
        let pageContext = self.pageContext
        if pageContext.isHidden {
            if !self.visibleSlices.isEmpty {
                let visibleSlices: [Slice] = self.visibleSlices
                self.visibleSlices.removeAll()
                self.dataSource.endAccess(slices: visibleSlices)
            }
        } else {
            var map: [String: Slice] = [:]
            _ = self.visibleSlices.map { (item) -> Void in
                map[item.info.identifier] = item
            }

            let topY: UInt32
            if pageContext.offset - Int32(self.sliceHeight) >= 0 {
                topY = UInt32(pageContext.offset - Int32(self.sliceHeight))
            } else {
                topY = 0
            }
            let bottomY: UInt32
            if pageContext.offset + Int32(pageContext.contentSize.height) >= 0 {
                bottomY = UInt32(pageContext.offset + Int32(pageContext.contentSize.height))
            } else {
                bottomY = 0
            }

            let lower = Int(topY / self.sliceHeight)
            var upper = Int((bottomY + self.sliceHeight - 1) / self.sliceHeight)
            if upper > self.slices.count - 1 {
                upper = self.slices.count - 1
            }
            
            if upper > lower {
                var removes: [Slice] = []
                self.visibleSlices = self.visibleSlices.filter { (item) -> Bool in
                    if item.info.index < upper || item.info.index > upper {
                        removes.append(item)
                        return false
                    } else {
                        return true
                    }
                }
                self.dataSource.endAccess(slices: removes)

                var visibleSlices: [Slice] = []
                if self.visibleSlices.isEmpty {
                    for index in lower ... upper {
                        visibleSlices.append(self.slices[index])
                    }
                    self.visibleSlices = visibleSlices
                    self.dataSource.beginAccess(slices: visibleSlices)
                } else {
                    var added: [Slice] = []

                    if lower < self.visibleSlices[0].info.index {
                        for index in lower ..< self.visibleSlices[0].info.index {
                            let item = self.slices[index]
                            visibleSlices.append(item)
                            added.append(item)
                        }
                    }
                    visibleSlices.append(contentsOf: self.visibleSlices)
                    if upper > self.visibleSlices.last!.info.index {
                        for index in self.visibleSlices.last!.info.index ... upper {
                            let item = self.slices[index]
                            visibleSlices.append(item)
                            added.append(item)
                        }
                    }
                    self.visibleSlices = visibleSlices
                    self.dataSource.beginAccess(slices: added)
                }
            } else {
                let visibleSlices: [Slice] = self.visibleSlices
                self.visibleSlices.removeAll()
                self.dataSource.endAccess(slices: visibleSlices)
            }
        }
    }
    
    
    
    deinit {
        self.pageContext.removeObserver(self.observer)
    }
    
}

