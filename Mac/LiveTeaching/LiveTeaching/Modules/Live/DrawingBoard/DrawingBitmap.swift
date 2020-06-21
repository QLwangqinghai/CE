//
//  DrawingBitmap.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/12.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class BitmapTileCell: BitmapTileContent {
    //相对于buffer中的位置
    public let origin: Point
    public let size: Size
    public let buffer: BitmapByteBuffer

    public init(buffer: BitmapByteBuffer, origin: Point, size: Size) {
        self.origin = origin
        self.buffer = buffer
        self.size = size
    }
    public func makeImage() -> CGImage? {
        return self.buffer.makeImage(origin: self.origin, size: self.size)
    }

    public func draw(in ctx: CGContext) {
        let bounds = ctx.boundingBoxOfClipPath
        let red = CGFloat(drand48())
        let green = CGFloat(drand48())
        let blue = CGFloat(drand48())
        ctx.setFillColor(UIColor(red: red, green: green, blue: blue, alpha: 1).cgColor)
        ctx.fill(bounds)
        
        print("draw(in ctx:), \(bounds)")
    }
    
}


public class TileManager {
    public let tiles: [TiledLine] = []
    public private(set) var visibleTiles: [Point: BitmapTile] = [:]

    //visibleFrame 一定是当前bounds 的一个子集
    public fileprivate(set) var visibleFrame: Rect {
        didSet(old) {
            if self.visibleFrame != old {

            }
        }
    }
    
    fileprivate class TileEntry {
        var value: BitmapTile?
        init(value: BitmapTile?) {
            self.value = value
        }
    }
    private let tileCache: [Point: TileEntry] = [:]

    public init() {
        self.visibleFrame = Rect.zero
    }
        
    open func tiles(in bounds: Rect) -> [(Rect, BitmapTile)] {
//        var result: [BitmapTile] = []
//        let xEnd = bounds.origin.x + bounds.size.width
//        let yEnd = bounds.origin.y + bounds.size.height
//
//        let xBegin = bounds.origin.x / BitmapTile.tileSize.width * BitmapTile.tileSize.width
//        let yBegin = bounds.origin.y / BitmapTile.tileSize.height * BitmapTile.tileSize.height
//
//        let lines = self.tiles.filter { (line) -> Bool in
//            return line.y >= yBegin && line.y < yEnd
//        }
//        lines.forEach { (line) in
//            line.items.forEach { (tile) in
//                if tile.origin.x >= xBegin && tile.origin.x < xEnd {
//                    result.append(tile)
//                }
//            }
//        }
        return []
    }


}


public class DrawingBitmap: BaseBitmap {
    //按照最低5mb/ms的memcpy速率设计
    
    public static let minChange: Point = {
        var p = Point(x: 1, y: 1)
        if UInt64(Int.max) == UInt64(Int64.max) {
            p.x = 2
        }
        return p
    }()
    
    
    public fileprivate(set) var contentOffset: Point {
        didSet(old) {
            if self.contentOffset != old {
                self.updateVisibleFrame(origin: self.contentOffset, size: self.size)
            }
        }
    }
    
    //visibleFrame 一定是当前bounds 的一个子集
    public fileprivate(set) var visibleFrame: Rect {
        didSet(old) {
            if self.visibleFrame != old {
                self.visibleFrameDidChange(from: old, to: self.visibleFrame)
            }
        }
    }
    
    private let tileManager: TileManager = TileManager()

    
    
    
    var onSequenceUpdate: ((DrawingBitmap) -> Void)? = nil
    public private(set) var sequence: UInt64 = 0
    public let sliceSize: Size
    

//    public var image: CGImage {
//        let bitmapLayout = self.status.bitmapLayout
//        let colorSpace = bitmapLayout.colorSpace
//        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//        return image
//    }
    
    public var contentSize: Size {
        didSet(old) {
            if old != self.contentSize {

                
            }
        }
    }


    public let backgroundColor: UInt32 = 0
    
//    //缓冲区
//    public let contentBuffer: BitmapByteBuffer
//    public let bitmapContext: CGContext
    
//    var bitmapSize = size
//    bitmapSize.width = (size.width + 31) / 32 * 32
//    let bytesPerRow = Int(BitmapInfo.littleArgb8888.bytesPerPixel) * Int(bitmapSize.width)
//    let buffer = BitmapByteBuffer(size: bitmapSize, bitmapInfo: BitmapInfo.littleArgb8888, byteCount: bytesPerRow * Int(bitmapSize.height), bytesPerRow: bytesPerRow)
//    self.bitmapContext = buffer.makeContext(origin: Point.zero, size: size)!

    
    
    public init(size: Size, contentSize: Size) {
        self.sliceSize = BitmapTile.tileSize
        self.contentSize = contentSize
        self.contentOffset = Point.zero
        self.visibleFrame = Rect(origin: Point.zero, size: size)
        super.init(size: size)
    }
    deinit {
//        self.status.removeObserver(self.observer)
    }
    

        
    open func updateVisibleFrame(origin: Point, size: Size) {
        self.update(visibleFrame: Rect(origin: origin, size: size))
    }
    open func update(visibleFrame: Rect) {
        let frame = visibleFrame.standardize()
        self.visibleFrame = frame
    }
    open func visibleFrameDidChange(from: Rect, to: Rect) {
        var visibleTiles: [Point: BitmapTile] = [:]
//        self.tiles(in: to).forEach { (tile) in
//            visibleTiles[tile.origin] = tile
//        }
//
//        self.visibleTiles.keys.forEach { (origin) in
//            if !set.contains(origin) {
//                self.visibleTiles.removeValue(forKey: origin)
//            }
//        }
//        set.forEach { (origin) in
//            if let _ = self.visibleTile[origin] {
//
//
//            } else {
//                self.visibleTiles[origin] =
//            }
//        }
//        self.visibleTiles.keys.forEach { (origin) in
//            if !set.contains(origin) {
//                self.visibleTiles.removeValue(forKey: origin)
//            }
//        }
    }

    
    
//    public func didUpdateStatus(from: DrawingStatus.Status, to: DrawingStatus.Status) {
//        self.contentHeight = to.contentHeight
//        
//        let sliceSize = self.status.drawingSize.cgSize
//        let top = to.offset > 0 ? to.offset : 0
//        let bottom = to.offset + sliceSize.height
//        
//        if top < self.layer.frame.origin.y || bottom > self.layer.frame.origin.y + self.layer.frame.size.height {
//            let t = top - (sliceSize.width - sliceSize.height) / 2
//            let y: UInt32
//            let scale = UIScreen.main.scale
//            if t < 0 {
//                y = 0
//            } else {
//                let tmp = Int(t * scale - 1)
//                y = tmp > 0 ? UInt32(tmp) : 0
//            }
//            
//            self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
//            self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)
//        }
//    }
//    
//    public func displayContent() {
//        let y: UInt32 = 0
//        let scale = UIScreen.main.scale
//        let sliceSize = self.status.drawingSize.cgSize
//
//        self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
//        self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)
//
////        if self.sequence > 5 {
////            for slice in self.slices {
////                let image = slice.cgImage
////                slice.cgImage = image
////                if let aimage = slice.cgImage  {
////
////                }
////                slice.setNeedsDisplay()
////            }
////            self.layer.setNeedsDisplay()
////            self.layer.superlayer?.setNeedsDisplay()
////            return
////        }
////        let bitmapLayout = self.status.bitmapLayout
////        let colorSpace = bitmapLayout.colorSpace
////        let bitmapInfo: UInt32 = colorSpace.bitmapInfo
////        for (idx, slice) in self.slices.enumerated() {
////            if idx < 16 {
////                slice.cgImage = CGImage(width: Int(self.size.width), height: Int(slice.size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: slice.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
////            } else {
////                slice.contents = nil
////            }
////        }
////        self.layer.setNeedsDisplay()
////
////        self.sequence += 1
//    }
//  
//    fileprivate func makeDataProvider(y: UInt32, height: UInt32) -> CGDataProvider {
//        let bitmapLayout = self.status.bitmapLayout
//        let offset = bitmapLayout.bytesPerRow * Int(y)
//        let size = bitmapLayout.bytesPerRow * Int(height)
//        return CGDataProvider(dataInfo: nil, data: self.ptr.advanced(by: offset), size: size) { (mptr, ptr, size) in
//            print("dataProvider release")
//            }!
//    }
//
//    func makeImage(y: UInt32, height: UInt32) -> CGImage? {
//        if height <= 0 {
//            return nil
//        }
//        if y >= self.contentHeight {
//            return nil
//        }
//        let bitmapLayout = self.status.bitmapLayout
//        let colorSpace = bitmapLayout.colorSpace
//        let bitmapInfo: UInt32 = colorSpace.bitmapInfo
//        let offset = bitmapLayout.bytesPerRow * Int(y)
//        let size = bitmapLayout.bytesPerRow * Int(height)
//        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr.advanced(by: offset), size: size) { (mptr, ptr, size) in
//            print("dataProvider release")
//            }!
//        return CGImage(width: Int(self.size.width), height: Int(height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
//    }
    
//    func makeImage(x: UInt32, y: UInt32, width: UInt32, height: UInt32) -> CGImage? {
//        let right = UInt64(x) + UInt64(width)
//        let bottom = UInt64(y) + UInt64(height)
//        if right > UInt64(self.size.width) {
//            return nil
//        }
//        if bottom > UInt64(self.size.height) {
//            return nil
//        }
//        let colorSpace = self.bitmapLayout.colorSpace
//        let bitmapInfo: UInt32 = colorSpace.bitmapInfo
//
//
//        let offset = self.bitmapLayout.bytesPerRow * Int(y) + Int(colorSpace.bytesPerPixel * x)
//        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr.advanced(by: offset), size: self.bufferSize - offset) { (mptr, ptr, size) in
//            print("de")
//            }!
//        return CGImage(width: Int(width), height: Int(height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: self.bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
//    }
    
}

