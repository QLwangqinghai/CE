//
//  DrawingView.swift
//  ScrollView
//
//  Created by vector on 2019/9/2.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

/*
 
  
 960 * 540
 810 * 540

 1280 * 720
 1080 * 720
 1920 * 1080
 1620 * 1080

 */

public class DrawingContext {
    public struct Config {
        public enum SizeMode {
            case preset960x540
            case preset810x540
            case preset1280x720
            case preset1080x720
            case preset1920x1080
            case preset1620x1080
            
            var size: (width: UInt32, height: UInt32) {
                switch self {
                case .preset960x540: return (960, 540)
                case .preset810x540: return (960, 540)
                case .preset1280x720: return (1280, 720)
                case .preset1080x720: return (1080, 720)
                case .preset1920x1080: return (1920, 1080)
                case .preset1620x1080: return (1620, 1080)
                }
            }
        }
        
        
        public let mode: SizeMode
//        public let blockSize: UInt32
//        public let blockPerRow: UInt32
//        public let numberOfRows: UInt32
        public let width: UInt32
        public let height: UInt32
        public let pageCount: UInt32
        public let pixelsCount: UInt32
        public let colorSpace: ColorSpace
        public var backgroundColor: Color = Color(little32Argb: 0)
        public let bufferSize: Int
        public let bytesPerRow: Int
        
        
        public init(mode: SizeMode, pageCount: UInt32, colorSpace: ColorSpace = .little32Argb) {
            self.mode = mode
//            let blockSize: UInt32 = 256
            let (width, height) = mode.size
            
//            var blockPerRow = (width + blockSize - 1) / blockSize
//            var numberOfRows = (height + blockSize - 1) / blockSize
//            blockPerRow = blockPerRow > 0 ? blockPerRow : 1
//            numberOfRows = numberOfRows > 0 ? numberOfRows : 1
//            let w = blockPerRow * blockSize
//            let h = numberOfRows * blockSize
            var count = pageCount
            if (count <= 0) {
                count = 1;
            }
            
            let pixelsCount = width * height * count
            let size: Int = Int(pixelsCount * colorSpace.bytesPerPixel)
//            self.blockSize = blockSize
//            self.blockPerRow = blockPerRow
//            self.numberOfRows = numberOfRows
            self.width = width
            self.height = height
            self.pageCount = count
            self.pixelsCount = pixelsCount
            self.bufferSize = size
            self.colorSpace = colorSpace
            self.bytesPerRow = Int(width * colorSpace.bytesPerPixel)
            print("w:\(width), h:\(height), bufferSize:\(self.bufferSize)")
        }
    }
    

    public enum ColorSpace: UInt32 {
        //android 只有argb模式
        
        case little32Argb = 1
        case little16Xrgb = 2
        public static let deviceRgb: CGColorSpace = CGColorSpaceCreateDeviceRGB()

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
        self.context = CGContext(data: mainPtr, width: Int(config.width), height: Int(config.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: config.bytesPerRow, space: DrawingContext.ColorSpace.deviceRgb, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
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
    
    func makeImage(x: UInt32, y: UInt32, width: UInt32, height: UInt32) -> CGImage? {
        let right = UInt64(x) + UInt64(width)
        let bottom = UInt64(y) + UInt64(height)
        if right > UInt64(self.config.width) {
            return nil
        }
        if bottom > UInt64(self.config.height) {
            return nil
        }
        let config = self.config
        let colorSpace = config.colorSpace
        let bitmapInfo: UInt32 = colorSpace.bitmapInfo

        let offset = config.bytesPerRow * Int(y) + Int(colorSpace.bytesPerPixel * x)
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr.advanced(by: offset), size: self.config.bufferSize - offset) { (mptr, ptr, size) in
            print("de")
            }!
        return CGImage(width: Int(width), height: Int(height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: config.bytesPerRow, space: DrawingContext.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
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
    
//    public override init(frame: CGRect) {
//        super.init(frame: frame)
//        let layer: CALayer =
//    }
    
//    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesBegan(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
//    }
//    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesMoved(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
//    }
//    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesEnded(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
//    }
//    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
//    }
//    
//    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
//    }
//    
    func resetContent() {
        
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
    
    open override var frame: CGRect {
        didSet(oldValue) {
            if self.frame.size != oldValue.size {
                self.contentView.resetContent()
            }
        }
    }
    open override var contentSize: CGSize {
        didSet(oldValue) {
            if self.contentSize != oldValue {
                self.contentView.resetContent()
            }
        }
    }
    open override var contentOffset: CGPoint {
        didSet(oldValue) {
            if self.contentOffset != oldValue {
                self.contentView.resetContent()
            }
        }
    }
    
    public override init(frame: CGRect) {
        self.contentView = DrawingView(frame: CGRect())
        super.init(frame: frame)
        self.addSubview(self.contentView)
    }
    
    required public init?(coder aDecoder: NSCoder) {
        self.contentView = DrawingView(frame: CGRect())
        super.init(coder: aDecoder)
        self.addSubview(self.contentView)
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
    
    func resetContent() {
        
    }
}

public struct Package {
    let x: UInt32
    let y: UInt32
    
    enum ChangeCode: UInt16 {
        case updateX = 1
        case updateY = 2
    }
    
    
}

public struct Change {
    let x: UInt32
    let y: UInt32
    
    
    
}


