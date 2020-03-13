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
    public static let chunkSize: Int32 = 128
    
    public let byteCount: Int
    public let ptr: UnsafeMutableRawPointer
    public let bitmapInfo: BitmapInfo
    public let size: Size
    public let bytesPerRow: Int

    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        assert(size.width % BitmapByteBuffer.chunkSize == 0)
        assert(size.height % BitmapByteBuffer.chunkSize == 0)
        
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
    
    public static let littleArgb8888: BitmapInfo = BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst, pixelFormatInfo: CGImagePixelFormatInfo.packed, orderInfo: CGImageByteOrderInfo.order32Little)
    
    public static let littleXrgb555: BitmapInfo = BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst, pixelFormatInfo: CGImagePixelFormatInfo.RGB555, orderInfo: CGImageByteOrderInfo.order16Little)
    
    public static let littleXrgb565: BitmapInfo = BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst, pixelFormatInfo: CGImagePixelFormatInfo.RGB565, orderInfo: CGImageByteOrderInfo.order16Little)

    
    public let bitsPerComponent: Int32
    public let bitsPerPixel: Int32
    public let bytesPerPixel: Int32
    public let space: CGColorSpace

    public let alphaInfo: CGImageAlphaInfo
    public let pixelFormatInfo: CGImagePixelFormatInfo
    public let orderInfo: CGImageByteOrderInfo
    
    public var bitmapInfo: UInt32 {
        return self.alphaInfo.rawValue | self.orderInfo.rawValue | self.pixelFormatInfo.rawValue
    }
    
    fileprivate init(bitsPerComponent: Int32, bitsPerPixel: Int32, bytesPerPixel: Int32, space: CGColorSpace, alphaInfo: CGImageAlphaInfo, pixelFormatInfo: CGImagePixelFormatInfo, orderInfo: CGImageByteOrderInfo) {
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

open class BaseBitmap: NSObject {
    public let size: Size
    public let layer: BitmapLayer
    fileprivate init(size: Size, layer: BitmapLayer) {
        assert(size.height > 0)
        assert(size.width > 0)

        self.size = size
        self.layer = layer
    }
}

open class BaseScrollBitmap: BaseBitmap {
    public fileprivate(set) var visibleFrame: Rect {
        didSet(old) {
            if self.visibleFrame != old {
                self.displayContent()
            }
        }
    }
    fileprivate init(size: Size, visibleFrame: Rect, layer: BitmapLayer) {
        self.visibleFrame = visibleFrame
        super.init(size: size, layer: layer)
    }
    open func displayContent() {
        fatalError("displayContent must override by subclass")
    }
    
    open func updateVisibleFrame(origin: Point, size: Size) -> Bool {
        if origin.x < 0 || origin.y < 0 {
            return false
        }
        if size.height < 0 || size.width < 0 {
            return false
        }
        if origin.x + size.width > self.size.width {
            return false
        }
        if origin.y + size.height > self.size.height {
            return false
        }
        self.visibleFrame = Rect(origin: origin, size: size)
        return true
    }
    open func update(visibleFrame: Rect) -> Bool {
        self.updateVisibleFrame(origin: visibleFrame.origin, size: visibleFrame.size)
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
        let layer: BitmapLayer = BitmapLayer()
        super.init(size: size, visibleFrame: Rect(), layer: layer)
    }
    
    open override func displayContent() {
        self.layer.cgImage = self.buffer.makeImage(frame: self.visibleFrame)
    }
//    public override func displayContent() {
//        self.layer.cgImage = self.makeImage(frame: visibleFrame)
//    }
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

public class BitmapChunk: BaseBitmap {
    //相对于bitmap整体中的位置
    public let origin: Point
    public var content: BitmapContent? = nil {
        didSet(old) {
            if old !== self.content {
                self.displayContent()
            }
        }
    }
    
    //size 宽高必须>0
    public init(frame: Rect) {
        self.origin = frame.origin
        let layer: BitmapLayer = BitmapLayer()
        super.init(size: frame.size, layer: layer)
    }
    open func displayContent() {
        guard let content = self.content else {
            self.layer.cgImage = nil
            return
        }
        self.layer.cgImage = content.makeImage()
    }
}

public class TiledScrollBitmap: BaseScrollBitmap {
    public let buffer: BitmapByteBuffer
    public let bitmapContext: CGContext
    public private(set) var chunks: [BitmapChunk] = []

    //size 宽高必须>0
    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        let buffer = BitmapByteBuffer(size: size, bitmapInfo: bitmapInfo)
        self.bitmapContext = buffer.makeContext(origin: Point(), size: size)!
        let layer: BitmapLayer = BitmapLayer()
        super.init(size: size, visibleFrame: Rect(), layer: layer)
    }
    
    open override func displayContent() {
        self.layer.cgImage = self.buffer.makeImage(frame: self.visibleFrame)
    }
//    public override func displayContent() {
//        self.layer.cgImage = self.makeImage(frame: visibleFrame)
//    }
}
