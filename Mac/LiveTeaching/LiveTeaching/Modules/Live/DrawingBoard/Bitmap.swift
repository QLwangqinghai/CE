//
//  Bitmap.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/11.
//  Copyright © 2020 vector. All rights reserved.
//

import CoreFoundation
import CoreGraphics
import UIKit


public final class BitmapByteBuffer {
    public let size: Size
    public let bitmapInfo: BitmapInfo
    public let byteCount: Int
    public let bytesPerRow: Int
    public let ptr: UnsafeMutableRawPointer

    public init(size: Size, bitmapInfo: BitmapInfo, byteCount: Int, bytesPerRow: Int) {
        assert(size.height > 0)
        assert(size.width > 0)
        assert(byteCount > 0)
        assert(bytesPerRow > 0)
        
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: byteCount, alignment: 0)
        self.size = size
        self.ptr = ptr
        self.bytesPerRow = bytesPerRow
        self.bitmapInfo = bitmapInfo
        self.byteCount = byteCount
    }
    
//    public convenience init(size: Size, bitmapInfo: BitmapInfo) {
//        assert(size.height > 0)
//        assert(size.width > 0)
//        
//        let bytesPerRow = Int(bitmapInfo.bytesPerPixel) * Int(size.width)
//        let byteCount = bytesPerRow * Int(size.height)
//        self.init(size: size, bitmapInfo: bitmapInfo, byteCount: byteCount, bytesPerRow: bytesPerRow)
//    }
    deinit {
        self.ptr.deallocate()
    }
    
    public func deepCopy() -> BitmapByteBuffer {
        let buffer = BitmapByteBuffer(size: self.size, bitmapInfo: self.bitmapInfo, byteCount: self.byteCount, bytesPerRow: self.bytesPerRow)
        memcpy(buffer.ptr, self.ptr, self.byteCount)
        return buffer
    }
    
    //frame 必须是当前bounds的子集
    internal func makeImage(frame: Rect) -> CGImage? {
        return self.makeImage(origin: frame.origin, size: frame.size)
    }
    internal func makeImage(origin: Point, size: Size) -> CGImage? {
        if origin.x <= 0 || origin.y <= 0 {
            return nil
        }
        if size.height <= 0 || size.width <= 0 {
            return nil
        }
        if origin.x + size.width > self.size.width {
            return nil
        }
        if origin.y + size.height > self.size.height {
            return nil
        }
        
        let bitmapInfo = self.bitmapInfo
        let length = self.bytesPerRow * Int(size.height - 1) + Int(size.width) * Int(bitmapInfo.bytesPerPixel)
        let bitsPerPixel = Int(bitmapInfo.bitsPerPixel)
        let offset = self.bytesPerRow * Int(origin.y) + bitsPerPixel * Int(origin.x)
        let bufferPointer: Unmanaged<BitmapByteBuffer> = Unmanaged.passRetained(self)
        let dataProvider = CGDataProvider(dataInfo: bufferPointer.toOpaque(), data: self.ptr.advanced(by: offset), size: length) { (dataInfo, data, size) in
            Unmanaged<BitmapByteBuffer>.fromOpaque(dataInfo!).release()
        }!
        return CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(bitmapInfo.bitsPerComponent), bitsPerPixel: Int(bitsPerPixel), bytesPerRow: bytesPerRow, space: bitmapInfo.space, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo.bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
    }

    internal func makeContext(origin: Point, size: Size) -> CGContext? {
        if origin.x <= 0 || origin.y <= 0 {
            return nil
        }
        if size.height <= 0 || size.width <= 0 {
            return nil
        }
        if origin.x + size.width > self.size.width {
            return nil
        }
        if origin.y + size.height > self.size.height {
            return nil
        }
        
        let bitmapInfo = self.bitmapInfo
        let bitsPerPixel = Int(bitmapInfo.bitsPerPixel)
        let offset = self.bytesPerRow * Int(origin.y) + bitsPerPixel * Int(origin.x)
        let bufferPointer: Unmanaged<BitmapByteBuffer> = Unmanaged.passRetained(self)
        return CGContext(data: self.ptr.advanced(by: offset), width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(bitmapInfo.bitsPerComponent), bytesPerRow: self.bytesPerRow, space: bitmapInfo.space, bitmapInfo: bitmapInfo.bitmapInfo, releaseCallback: { (releaseInfo, data) in
            Unmanaged<BitmapByteBuffer>.fromOpaque(releaseInfo!).release()
        }, releaseInfo: bufferPointer.toOpaque())
    }
}

public final class BitmapInfo: Hashable {
    public static let colorSpace: CGColorSpace = CGColorSpaceCreateDeviceRGB()
    
    public static let littleArgb8888: BitmapInfo = {
        if #available(iOS 12, *) {
            return BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst.rawValue, pixelFormatInfo: CGImagePixelFormatInfo.packed.rawValue, orderInfo: CGImageByteOrderInfo.order32Little.rawValue)
        } else {
            return BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst.rawValue, pixelFormatInfo: 0, orderInfo: CGImageByteOrderInfo.order32Little.rawValue)
        }
    }()
    
    public static let littleXrgb555: BitmapInfo = {
        if #available(iOS 12, *) {
            return BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst.rawValue, pixelFormatInfo: CGImagePixelFormatInfo.RGB555.rawValue, orderInfo: CGImageByteOrderInfo.order16Little.rawValue)
        } else {
            return BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst.rawValue, pixelFormatInfo: 0, orderInfo: CGImageByteOrderInfo.order16Little.rawValue)
        }
   }()
        
    @available(iOS 12.0, *)
    public static let littleXrgb565: BitmapInfo = BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst.rawValue, pixelFormatInfo: CGImagePixelFormatInfo.RGB565.rawValue, orderInfo: CGImageByteOrderInfo.order16Little.rawValue)

    
    public let bitsPerComponent: Int32
    public let bitsPerPixel: Int32
    public let bytesPerPixel: Int32
    public let space: CGColorSpace

    public let alphaInfo: UInt32
    public let pixelFormatInfo: UInt32
    public let orderInfo: UInt32
    
    public var bitmapInfo: UInt32 {
        return self.alphaInfo | self.orderInfo | self.pixelFormatInfo
    }
    
    fileprivate init(bitsPerComponent: Int32, bitsPerPixel: Int32, bytesPerPixel: Int32, space: CGColorSpace, alphaInfo: UInt32, pixelFormatInfo: UInt32, orderInfo: UInt32) {
        self.bitsPerComponent = bitsPerComponent
        self.bitsPerPixel = bitsPerPixel
        self.bytesPerPixel = bytesPerPixel
        self.space = space
        self.alphaInfo = alphaInfo
        self.pixelFormatInfo = pixelFormatInfo
        self.orderInfo = orderInfo
    }
    
    public static func == (lhs: BitmapInfo, rhs: BitmapInfo) -> Bool {
        return lhs.bitsPerComponent == rhs.bitsPerComponent && lhs.bitsPerPixel == rhs.bitsPerPixel && lhs.space == rhs.space && lhs.alphaInfo == rhs.alphaInfo && lhs.pixelFormatInfo == rhs.pixelFormatInfo && lhs.orderInfo == rhs.orderInfo
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.bitsPerComponent)
        hasher.combine(self.bitsPerPixel)
    }
}

open class BitmapLayer: CALayer {
    public var cgImage: CGImage? {
        didSet {
            self.contents = self.cgImage
        }
    }
    open override func draw(in ctx: CGContext) {
        guard let cgImage = self.cgImage else {
            return
        }
        ctx.draw(cgImage, in: self.bounds)
    }
}

open class BaseBitmap {
    public let size: Size
    internal init(size: Size) {
        assert(size.height > 0)
        assert(size.width > 0)
        self.size = size
    }
}

public class ARGBBitmap: BaseBitmap {
    public let buffer: BitmapByteBuffer
    public let bitmapContext: CGContext
    
    //size 宽高必须>0
    public override init(size: Size) {
        assert(size.height > 0)
        assert(size.width > 0)

        let bitmapInfo = BitmapInfo.littleArgb8888
        let bytesPerRow = Int(bitmapInfo.bytesPerPixel) * Int(size.width)
        let byteCount = bytesPerRow * Int(size.height)
        let buffer = BitmapByteBuffer(size: size, bitmapInfo: bitmapInfo, byteCount: byteCount, bytesPerRow: bytesPerRow)
        self.buffer = buffer
        self.bitmapContext = buffer.makeContext(origin: Point.zero, size: size)!
        super.init(size: size)
    }
}

//16位 bitmap, 不支持透明通道
public class RGBBitmap: BaseBitmap {
    
    @available(iOS 12.0, *)
    public enum Mode {
        case littleXrgb555
        case littleRgb565
    }
    
    public enum Background {
        case color(r: UInt8, g: UInt8, b: UInt8)
        //纹理
        case pattern(image: CGImage)
    }
//    public let backgroundColor: UIColor
//    private let _backgroundColor: UInt32
    public let background: Background
    public let buffer: BitmapByteBuffer
    public let bitmapContext: CGContext
    
    //size 宽高必须>0
    
    private init(size: Size, background: Background, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        let bytesPerRow = Int(bitmapInfo.bytesPerPixel) * Int(size.width)
        let byteCount = bytesPerRow * Int(size.height)
        let buffer = BitmapByteBuffer(size: size, bitmapInfo: bitmapInfo, byteCount: byteCount, bytesPerRow: bytesPerRow)
        self.buffer = buffer
        self.bitmapContext = buffer.makeContext(origin: Point.zero, size: size)!
        self.background = background
        super.init(size: size)
    }
    
    @available(iOS 12.0, *)
    public convenience init(size: Size, background: Background, mode: Mode) {
        assert(size.height > 0)
        assert(size.width > 0)
        
        let bitmapInfo: BitmapInfo
        switch mode {
        case .littleXrgb555:
            bitmapInfo = BitmapInfo.littleXrgb555
            break
        case .littleRgb565:
            bitmapInfo = BitmapInfo.littleXrgb565
            break
        }
        self.init(size: size, background:background, bitmapInfo: bitmapInfo)
    }
    
    public convenience init(size: Size, background: Background) {
        assert(size.height > 0)
        assert(size.width > 0)
        self.init(size: size, background:background, bitmapInfo: BitmapInfo.littleXrgb555)
    }
}

public protocol BitmapTileContent: class {
    func draw(in ctx: CGContext)
}



public class BitmapTile {
    public struct IndexPath: Hashable {
        public var x: Int32
        public var y: Int32

        public init(x: Int32, y: Int32) {
            self.x = x
            self.y = y
        }
        public init(origin: Point) {
            let px = origin.x
            var x = px / tileSize.width
            if px < 0 {
                if px % tileSize.width != 0 {
                    x -= 1
                }
            }
            let py = origin.y
            var y = py / tileSize.height
            if py < 0 {
                if py % tileSize.height != 0 {
                    y -= 1
                }
            }
            
            self.x = x
            self.y = y
        }

        public static func == (_ lhs: IndexPath, _ rhs: IndexPath) -> Bool {
            return lhs.x == rhs.x && lhs.y == rhs.y
        }
        public func hash(into hasher: inout Hasher) {
            hasher.combine(self.x)
            hasher.combine(self.y)
        }
    }
    
    
    public static let tileSize: Size = {
        let scale = UIScreen.main.scale
        if scale < 1.1 {
            return Size(width: 128, height: 128)
        } else if scale < 2.1 {
            return Size(width: 256, height: 256)
        } else if scale < 3.1 {
            return Size(width: 384, height: 384)
        } else {
            return Size(width: 256, height: 256)
        }
    }()
    
    public static func sizeAlignment(size: Size) -> Size {
        var result = Size.zero
        if size.width >= 0 {
            result.width = (size.width + tileSize.width - 1) / tileSize.width * tileSize.width
        } else {
            result.width = (size.width - tileSize.width + 1) / tileSize.width * tileSize.width
        }
        if size.height >= 0 {
            result.height = (size.height + tileSize.height - 1) / tileSize.height * tileSize.height
        } else {
            result.height = (size.height - tileSize.height + 1) / tileSize.height * tileSize.height
        }
        return result
    }
    
    //相对于bitmap整体中的位置
    public let indexPath: IndexPath

    public var content: BitmapTileContent? = nil
    //size 宽高必须>0
    public init(indexPath: IndexPath) {
        self.indexPath = indexPath
    }
}

public class TiledLine {
    public let y: Int32
    public fileprivate(set) var items: [BitmapTile] = []
    init(y: Int32) {
        self.y = y
    }
}


public protocol TiledBitmapDataSource: class {

    
    
}

//public class TiledBitmap: BaseBitmap {
//    //visibleFrame 一定是当前bounds 的一个子集
//    public fileprivate(set) var visibleFrame: Rect {
//        didSet(old) {
//            if self.visibleFrame != old {
//                self.visibleFrameDidChange(from: old, to: self.visibleFrame)
//            }
//        }
//    }
//
//    public let tiles: [TiledLine]
//    public private(set) var visibleTiles: [Point: BitmapTile]
//
//    //size 宽高必须>0
//    public init(size: Size, background: Background, visibleFrame: Rect, bitmapInfo: BitmapInfo) {
//        assert(size.height > 0)
//        assert(size.width > 0)
//
//        var yRow: Int32 = 0
//        let tileSize = BitmapTile.tileSize
//        var lines: [TiledLine] = []
//
//        while yRow < size.height {
//            let line = TiledLine(y: yRow)
//            var xRow: Int32 = 0
//            while xRow < size.width {
//                let tile = BitmapTile(origin: Point(x: xRow, y: yRow), size: tileSize)
//                line.items.append(tile)
//                xRow += tileSize.width
//            }
//            lines.append(line)
//            yRow += tileSize.height
//        }
//        self.tiles = lines
//        self.visibleFrame = visibleFrame
//        super.init(size: size, background:background)
//    }
//
//    open func updateVisibleFrame(origin: Point, size: Size) {
//        self.update(visibleFrame: Rect(origin: origin, size: size))
//    }
//    open func update(visibleFrame: Rect) {
//        let frame = visibleFrame.standardize()
//        self.visibleFrame = frame
//    }
//    open func visibleFrameDidChange(from: Rect, to: Rect) {
//        var visibleTiles: [Point: BitmapTile] = [:]
//        self.tiles(in: to).forEach { (tile) in
//            visibleTiles[tile.origin] = tile
//        }
//
////        self.visibleTiles.keys.forEach { (origin) in
////            if !set.contains(origin) {
////                self.visibleTiles.removeValue(forKey: origin)
////            }
////        }
////        set.forEach { (origin) in
////            if let _ = self.visibleTile[origin] {
////
////
////            } else {
////                self.visibleTiles[origin] =
////            }
////        }
////        self.visibleTiles.keys.forEach { (origin) in
////            if !set.contains(origin) {
////                self.visibleTiles.removeValue(forKey: origin)
////            }
////        }
//    }
//
//    open func tiles(in bounds: Rect) -> [BitmapTile] {
//        var result: [BitmapTile] = []
//        let xEnd = bounds.origin.x + bounds.size.width
//        let yEnd = bounds.origin.y + bounds.size.height
//
//        let xBegin = bounds.origin.x / BitmapTile.tileSize * BitmapTile.tileSize
//        let yBegin = bounds.origin.y / BitmapTile.tileSize * BitmapTile.tileSize
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
//    }
//}
