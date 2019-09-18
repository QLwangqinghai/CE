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
            var blockPerRow = (width + blockSize - 1) / blockSize
            var numberOfRows = (height + blockSize - 1) / blockSize
            blockPerRow = blockPerRow > 0 ? blockPerRow : 1
            numberOfRows = numberOfRows > 0 ? numberOfRows : 1
            
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
            
            print("w:\(width), h:\(height), bufferSize:\(self.bufferSize)")
        }
    }
    

    public enum ColorSpace: UInt32 {
        case little32Argb = 1
        case little16Xrgb = 2
        
        public var bytesPerPixel: UInt32 {
            switch self {
            case .little32Argb:
                return 4
            case .little16Xrgb:
                return 2
            }
        }
        public var bitsPerComponent: Int {
            switch self {
            case .little32Argb:
                return 8
            case .little16Xrgb:
                return 5
            }
        }
        public var bitsPerPixel: Int {
            switch self {
            case .little32Argb:
                return 32
            case .little16Xrgb:
                return 16
            }
        }
        
        
        public var bitmapInfo: UInt32 {
            switch self {
            case .little32Argb:
                return CGBitmapInfo.byteOrder32Little.rawValue | CGImageAlphaInfo.premultipliedFirst.rawValue
            case .little16Xrgb:
                return CGBitmapInfo.byteOrder16Little.rawValue | CGImageAlphaInfo.noneSkipFirst.rawValue
            }
        }
        
        
    }
    
   /*
     RGB
     16 bpp, 5 bpc, kCGImageAlphaNoneSkipFirst
     Mac OS X, iOS
     RGB
     32 bpp, 8 bpc, kCGImageAlphaNoneSkipFirst
     Mac OS X, iOS
     RGB
     32 bpp, 8 bpc, kCGImageAlphaNoneSkipLast
     Mac OS X, iOS
     RGB
     32 bpp, 8 bpc, kCGImageAlphaPremultipliedFirst
     Mac OS X, iOS
     RGB
     32 bpp, 8 bpc, kCGImageAlphaPremultipliedLast
     Mac OS X, iOS
     */ //https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB
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
    public let size: CGSize

    public init(config: Config, size: CGSize) {
        let mainPtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.mainPtr = mainPtr
        self.duplicatePtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.config = config
        let colorSpace = config.colorSpace
        self.size = size
        self.context = CGContext(data: mainPtr, width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: Int(config.width * config.colorSpace.bytesPerPixel), space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
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
        let colorSpace = self.config.colorSpace
        let bitmapInfo: UInt32 = colorSpace.bitmapInfo

        return CGImage(width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: Int(config.width * colorSpace.bytesPerPixel), space: CGColorSpaceCreateDeviceRGB(), bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
    }
    
}




public protocol Stroke: class {
    
}


open class DrawingView: UIView {
    fileprivate var context: DrawingContext? = nil {
        didSet {
            self.layer.contents = context?.image
            self.setNeedsDisplay()
        }
    }
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesBegan(touches, with: event)
        self.log(item: "\(touches) \(String(describing: event))")
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesMoved(touches, with: event)
        self.log(item: "\(touches) \(String(describing: event))")
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesEnded(touches, with: event)
        self.log(item: "\(touches) \(String(describing: event))")
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesCancelled(touches, with: event)
        self.log(item: "\(touches) \(String(describing: event))")
    }
    
    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
        print("\(file) :\(line): \(function) \(item())")
    }
    
}

open class DrawingScrollView: UIScrollView {
    public var contentView: DrawingView
    var array: [CGImage?] = []
    
    public var context: DrawingContext? = nil {
        didSet {
            self.contentView.context = self.context
            
            let size: CGSize
            if let v = self.context?.size {
                size = v
            } else {
                size = CGSize()
            }
            
            self.contentView.frame.size = size
            self.contentSize = self.contentView.frame.size
            self.panGestureRecognizer.minimumNumberOfTouches = 2
        }
    }
    

    public override init(frame: CGRect) {
        self.contentView = DrawingView(frame: CGRect())
        super.init(frame: frame)
        self.addSubview(self.contentView)
    }
    
    required public init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
}
