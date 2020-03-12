//
//  Bitmap.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/11.
//  Copyright © 2020 vector. All rights reserved.
//

import CoreFoundation
import CoreGraphics


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
    public let visibleFrame: Rect

    fileprivate init(size: Size, visibleFrame: Rect, layer: BitmapLayer) {
        self.visibleFrame = visibleFrame
        self.size = size
        self.layer = layer
    }
}

public class BaseScrollBitmap: BaseBitmap {

    public func displayContent() {
//        let y: UInt32 = 0
//        let scale = UIScreen.main.scale
//        let sliceSize = self.status.drawingSize.cgSize
//
//        self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
//        self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)
    }
}


public class ScrollBitmap: BaseScrollBitmap {
    public let bufferSize: Int
    public let ptr: UnsafeMutableRawPointer
    public let bitmapContext: CGContext
//    public var image: CGImage {
//        self.bitmapContext.colorSpace
////        bytesPerRow
//        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//
//
//        let bitmapLayout = self.bitmapLayout
//        let colorSpace = bitmapLayout.colorSpace
//        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//        return image
//    }
    public private(set) var slices: [DrawingBitmapSliceLayer] = []
    public let dataProvider: CGDataProvider

    
    //size 宽高必须>0
    public init(size: Size, bitmapLayout: Drawing.BitmapLayout) {
        assert(size.height > 0)
        assert(size.width > 0)
        assert(size.width <= bitmapLayout.countPerRow)

//        CGImagePixelFormatInfo;

        
        let bufferSize = bitmapLayout.bytesPerRow * Int(size.height)
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: CCGetCachelineSize())
        self.ptr = ptr
        self.bufferSize = bufferSize
        
        let colorSpace = bitmapLayout.colorSpace
        let bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.bitmapContext = bitmapContext
        let layer: BitmapLayer = BitmapLayer()
        layer.zPosition = -100
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr, size: self.bufferSize) { (mptr, ptr, size) in
            print("de")
            }!
        self.dataProvider = dataProvider
        super.init(size: size, visibleFrame: Rect(), layer: layer)
    }
    deinit {
//        self.status.removeObserver(self.observer)
    }
    
    public override func displayContent() {
//        let y: UInt32 = 0
//        let scale = UIScreen.main.scale
//        let sliceSize = self.status.drawingSize.cgSize
//
//        self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
//        self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)
    }
    
    
    //frame 必须是当前bounds的子集
    private func makeImage(frame: Rect) -> CGImage? {
        if frame.size.height <= 0 || frame.size.width <= 0 {
            return nil
        }
        if frame.origin.x <= 0 || frame.origin.y <= 0 {
            return nil
        }
        if frame.origin.x + frame.size.width > self.size.width {
            return nil
        }
        if frame.origin.y + frame.size.height > self.size.height {
            return nil
        }
        let bytesPerRow = self.bitmapContext.bytesPerRow
        let bitsPerPixel = self.bitmapContext.bitsPerPixel
        let colorSpace = self.bitmapContext.colorSpace ?? CGColorSpaceCreateDeviceRGB()
        let bitmapInfo: UInt32 = self.bitmapContext.bitmapInfo.rawValue
        let offset = bytesPerRow * Int(frame.origin.y) + bitsPerPixel * Int(frame.origin.x)
        let size = bytesPerRow * Int(frame.size.height)
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr.advanced(by: offset), size: size) { (mptr, ptr, size) in
            print("dataProvider release")
            }!
        return CGImage(width: Int(frame.size.width), height: Int(frame.size.height), bitsPerComponent: self.bitmapContext.bitsPerComponent, bitsPerPixel: bitsPerPixel, bytesPerRow: bytesPerRow, space: colorSpace, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
    }
    
}
