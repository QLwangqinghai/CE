//
//  Bitmap.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/11.
//  Copyright © 2020 vector. All rights reserved.
//

import CoreFoundation
import CoreGraphics


public class BitmapByteBuffer {
    public let byteCount: Int
    public let ptr: UnsafeMutableRawPointer
    public let bitmapInfo: BitmapInfo
    public let size: Size
    public let bytesPerRow: Int

    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        
        let bytesPerRow = Int(bitmapInfo.bytesPerPixel) * Int(size.width)
        let bufferSize = bytesPerRow * Int(size.height)
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: 0)
        self.size = size
        self.ptr = ptr
        self.bytesPerRow = bytesPerRow
        self.bitmapInfo = bitmapInfo
        self.byteCount = bufferSize
    }
    deinit {
        self.ptr.deallocate()
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
        if #available(iOS 13, *) {
            return BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst.rawValue, pixelFormatInfo: CGImagePixelFormatInfo.packed.rawValue, orderInfo: CGImageByteOrderInfo.order32Little.rawValue)
        } else {
            return BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst.rawValue, pixelFormatInfo: 0, orderInfo: CGImageByteOrderInfo.order32Little.rawValue)
        }
    }()
    
    public static let littleXrgb555: BitmapInfo = {
        if #available(iOS 13, *) {
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
    fileprivate init(size: Size) {
        assert(size.height > 0)
        assert(size.width > 0)
        self.size = size
    }
}

open class BaseScrollBitmap: BaseBitmap {
    public let content: BitmapLayer
    //visibleFrame 一定是当前bounds 的一个子集
    public fileprivate(set) var visibleFrame: Rect {
        didSet(old) {
            if self.visibleFrame != old {
                self.visibleFrameDidChange(from: old, to: self.visibleFrame)
            }
        }
    }
    fileprivate init(size: Size, visibleFrame: Rect) {
        self.visibleFrame = visibleFrame
        super.init(size: size)
    }
    
    open func updateVisibleFrame(origin: Point, size: Size) {
        self.update(visibleFrame: Rect(origin: origin, size: size))
    }
    open func update(visibleFrame: Rect) {
        let frame = visibleFrame.standardize()
        self.visibleFrame = frame
    }
    open func visibleFrameDidChange(from: Rect, to: Rect) {
    }
}


public class ScrollBitmap: BaseScrollBitmap {
    public let buffer: BitmapByteBuffer
    public let bitmapContext: CGContext
    
    //size 宽高必须>0
    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        
        let buffer = BitmapByteBuffer(size: size, bitmapInfo: bitmapInfo)
        self.bitmapContext = buffer.makeContext(origin: Point(), size: size)!
        super.init(size: size, visibleFrame: Rect())
    }
}

public class BitmapContent {
    //相对于buffer中的位置
    public let origin: Point
    public let size: Size
    public let buffer: BitmapByteBuffer

    public init(buffer: BitmapByteBuffer, origin: Point, size: Size) {
        self.origin = origin
        self.buffer = buffer
        self.size = size
    }
    internal func makeImage() -> CGImage? {
        return self.buffer.makeImage(origin: self.origin, size: self.size)
    }
}

public class BitmapTile {
    public static let tileSize: Int32 = 256
    public static func sizeAlignment(size: Size) -> Size {
        var result = Size()
        if size.width >= 0 {
            result.width = (size.width + tileSize - 1) / tileSize * tileSize
        } else {
            result.width = (size.width - tileSize + 1) / tileSize * tileSize
        }
        if size.height >= 0 {
            result.height = (size.height + tileSize - 1) / tileSize * tileSize
        } else {
            result.height = (size.height - tileSize + 1) / tileSize * tileSize
        }
        return result
    }
    
    //相对于bitmap整体中的位置
    public let origin: Point
    public let size: Size

    public var content: BitmapContent? = nil
    //size 宽高必须>0
    public init(origin: Point, size: Size) {
        assert(size.width > 0)
        assert(size.height > 0)

        assert(size.width % BitmapTile.tileSize == 0)
        assert(size.height % BitmapTile.tileSize == 0)

        self.origin = origin
        self.size = size
    }
}


public class TiledLine {
    public let y: Int32
    public fileprivate(set) var items: [BitmapTile] = []
    init(y: Int32) {
        self.y = y
    }
}

public class TiledScrollBitmap: BaseScrollBitmap {
    public let tiles: [TiledLine]
    public private(set) var visibleTiles: [Point: BitmapTile]

    //size 宽高必须>0
    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        
        var yRow: Int32 = 0
        
        let tileSize = Size(width: BitmapTile.tileSize, height: BitmapTile.tileSize)
        var lines: [TiledLine] = []

        while yRow < size.height {
            let line = TiledLine(y: yRow)
            var xRow: Int32 = 0
            while xRow < size.width {
                let tile = BitmapTile(origin: Point(x: xRow, y: yRow), size: tileSize)
                line.items.append(tile)
                xRow += tileSize.width
            }
            lines.append(line)
            yRow += tileSize.height
        }
        self.tiles = lines
        super.init(size: size, visibleFrame: Rect())
    }
    open override func visibleFrameDidChange(from: Rect, to: Rect) {
        let xEnd = to.origin.x + to.size.width
        let yEnd = to.origin.y + to.size.height

        let xBegin = to.origin.x / BitmapTile.tileSize * BitmapTile.tileSize
        let yBegin = to.origin.y / BitmapTile.tileSize * BitmapTile.tileSize
        var visibleTiles: [Point: BitmapTile] = [:]
        
        let lines = self.tiles.filter { (line) -> Bool in
            return line.y >= yBegin && line.y < yEnd
        }
        lines.forEach { (line) in
            line.items.forEach { (tile) in
                if tile.origin.x >= xBegin && tile.origin.x < xEnd {
                    visibleTiles[tile.origin] = tile
                }
            }
        }
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
}
