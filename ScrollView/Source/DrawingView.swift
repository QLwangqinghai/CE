//
//  DrawingView.swift
//  ScrollView
//
//  Created by vector on 2019/9/2.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit



public class DrawingContext {
    public struct Config {
        public let blockSize: UInt32
        public let blockPerRow: UInt32
        public let numberOfRows: UInt32
        public let width: UInt32
        public let height: UInt32
        public let pixelsCount: UInt32
        public let colorSpace: ColorSpace
        public var backgroundColor: Color = Color(little32Argb: 0)
        public let bufferSize: Int

        public init(width: UInt32, height: UInt32, colorSpace: ColorSpace = .little32Argb) {
            let blockSize: UInt32 = 256
            let blockPerRow = (width + blockSize - 1) / blockSize
            let numberOfRows = (height + blockSize - 1) / blockSize
            let w = blockPerRow * blockSize
            let h = numberOfRows * blockSize
            let pixelsCount = w * h
            let size: Int = Int(pixelsCount * colorSpace.bytesPerPixel)
            self.blockSize = blockSize
            self.blockPerRow = blockPerRow
            self.numberOfRows = numberOfRows
            self.width = w
            self.height = h
            self.pixelsCount = pixelsCount
            self.bufferSize = size
            self.colorSpace = colorSpace
            
            print("bufferSize:\(self.bufferSize)")
        }
    }
    

    public enum ColorSpace: UInt32 {
        case little32Argb = 1
        
        public var bytesPerPixel: UInt32 {
            switch self {
            case .little32Argb:
                return 4
            }
        }
        public var bitsPerComponent: Int {
            switch self {
            case .little32Argb:
                return 8
            }
        }
        public var bitsPerPixel: Int {
            switch self {
            case .little32Argb:
                return 32
            }
        }
        
        
        public var bitmapInfo: UInt32 {
            switch self {
            case .little32Argb:
                return CGBitmapInfo.byteOrder32Little.rawValue | CGImageAlphaInfo.premultipliedFirst.rawValue
            }
        }
        
        
    }
    public struct Color {
        public let color32: UInt32
        public let color16: UInt16
        public init(little32Argb: UInt32) {
            self.color32 = little32Argb
            let a = little32Argb & 0xf0_00_00_00
            let r = little32Argb & 0x00_f0_00_00
            let g = little32Argb & 0x00_00_f0_00
            let b = little32Argb & 0x00_00_00_f0
            self.color16 = UInt16(clamping: (a >> 16) + (r >> 12) + (g >> 8) + (b >> 4))
        }
        public init(little16Argb: UInt16) {
            self.color16 = little16Argb
            let c: UInt32 = UInt32(little16Argb)
            let a = c & 0x00_00_f0_00
            let r = c & 0x00_00_0f_00
            let g = c & 0x00_00_00_f0
            let b = c & 0x00_00_00_0f
            self.color32 = ((a << 16) + (r << 12) + (g << 8) + (b << 4))
        }
    }
    
    public let mainPtr: UnsafeMutableRawPointer
    public let duplicatePtr: UnsafeMutableRawPointer
    public let config: Config
    public var image: CGImage? = nil
    public let context: CGContext
    public let dataProvider: CGDataProvider
    
    public init(config: Config) {
        let mainPtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.mainPtr = mainPtr
        self.duplicatePtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.config = config
        let colorSpace = config.colorSpace
        
        self.context = CGContext(data: mainPtr, width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: Int(config.width * 4), space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        
        self.dataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr, size: self.config.bufferSize) { (mptr, ptr, size) in
            print("de")
            }!

        
        
//        let callback = CGDataProviderDirectCallbacks.init(version: 0, getBytePointer: { (ptr) -> UnsafeRawPointer? in
//            return ptr
//        }, releaseBytePointer: { (context, ptr) in
//
//        }, getBytesAtPosition: { (context, ptr, offset, size) -> Int in
//
//        }) { (context) in
//
//        }
//
//
//
//        CGDataProvider.init(directInfo: <#T##UnsafeMutableRawPointer?#>, size: <#T##off_t#>, callbacks: <#T##UnsafePointer<CGDataProviderDirectCallbacks>#>)
//        let dataProvider: CGDataProvider = CGDataProvider.init(directInfo: nil, size: 0, callbacks: UnsafePointer<CGDataProviderDirectCallbacks>)
//
        
//        switch self.config.colorSpace {
//        case .little32Argb:
//            C2DLittle32ArgbPixelsSet(self.mainPtr, self.config.backgroundColor.color32, self.config.pixelsCount)
//            C2DLittle32ArgbPixelsSet(self.mainPtr, self.config.backgroundColor.color32, self.config.pixelsCount)
//        }

//        let dataProvider: CGDataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr, size: self.config.bufferSize) { (mptr, ptr, size) in
//            print("de")
//        }!

//        var bitmapInfo: UInt32 = 0
//        bitmapInfo |= CGImageAlphaInfo.premultipliedFirst.rawValue
//        bitmapInfo |= CGBitmapInfo.byteOrder32Little.rawValue
//
//        self.image = CGImage(width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: Int(config.width * 4), space: CGColorSpaceCreateDeviceRGB(), bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//
//        switch self.config.colorSpace {
//        case .little32Argb:
//            C2DLittle32ArgbPixelsSet(self.mainPtr, self.config.backgroundColor.color32, self.config.pixelsCount)
//            C2DLittle32ArgbPixelsSet(self.mainPtr, self.config.backgroundColor.color32, self.config.pixelsCount)
//        }
    }
    
    func makeImage() -> CGImage? {
        var bitmapInfo: UInt32 = 0
        bitmapInfo |= CGImageAlphaInfo.premultipliedFirst.rawValue
        bitmapInfo |= CGBitmapInfo.byteOrder32Little.rawValue
        let colorSpace = self.config.colorSpace
        return CGImage(width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: Int(config.width * 4), space: CGColorSpaceCreateDeviceRGB(), bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
    }
    
}




open class DrawingView: UIView {
    public var context: DrawingContext? = nil {
        didSet {
            self.layer.contents = context?.image
            self.setNeedsDisplay()
        }
    }
    

    
    
}

open class DrawingScrollView: UIScrollView {
    public var contentView: DrawingView
    var array: [CGImage?] = []

    public override init(frame: CGRect) {
        let w: UInt32 = 256 * 6
        let h: UInt32 = 256 * 4 * 10

        self.contentView = DrawingView(frame: CGRect(x: 0, y: 0, width: CGFloat(w) / UIScreen.main.scale, height: CGFloat(h) / UIScreen.main.scale))
        super.init(frame: frame)
        var config = DrawingContext.Config(width: w, height: h, colorSpace: .little32Argb)
        config.backgroundColor = DrawingContext.Color.init(little32Argb: arc4random())
        let context: DrawingContext = DrawingContext(config: config)
        self.addSubview(self.contentView)
        self.contentView.context = context
        
        self.contentSize = self.contentView.frame.size
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 1) {
            if let context = self.contentView.context {
                C2DLittle32ArgbPixelsSet(context.mainPtr, 0xff_ff_00_00, context.config.pixelsCount)
                self.contentView.layer.contents = context.makeImage()
                self.contentView.setNeedsDisplay()
            }
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
            if let context = self.contentView.context {
                for _ in 1 ..< 100 {
                    self.array.append(context.makeImage())
                }
                C2DLittle32ArgbPixelsSet(context.mainPtr, arc4random() | 0x80_00_00_00, context.config.pixelsCount)
                self.contentView.layer.contents = context.makeImage()
                self.contentView.setNeedsDisplay()
            }
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 5) {
            if let context = self.contentView.context {
                C2DLittle32ArgbPixelsSet(context.mainPtr, arc4random() | 0x80_00_00_00, context.config.pixelsCount)
                self.contentView.layer.contents = context.makeImage()
                self.contentView.setNeedsDisplay()
            }
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 7) {
            if let context = self.contentView.context {
                C2DLittle32ArgbPixelsSet(context.mainPtr, arc4random() | 0x80_00_00_00, context.config.pixelsCount)
                self.contentView.layer.contents = context.makeImage()
                self.contentView.setNeedsDisplay()
            }
        }
    }
    
    required public init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
}
