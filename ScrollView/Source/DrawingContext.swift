//
//  DrawingContext.swift
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


//http://blog.sina.com.cn/s/blog_894d45e20102wwrt.html

public class DrawingContext {
    
    
    var displaylink: CADisplayLink?
    
    func createDisplayLink() {
        // 创建CADisplayLink
        displaylink = CADisplayLink(target: self,
                                    selector: #selector(DrawingContext.onDisplaylink))
        
        displaylink!.add(to: RunLoop.main, forMode: RunLoop.Mode.common)
    }
    
    
//    open var timestamp: CFTimeInterval { get }
//
//    open var duration: CFTimeInterval { get }
//
//
//    /* The next timestamp that the client should target their render for. */
//
//    @available(iOS 10.0, *)
//    open var targetTimestamp: CFTimeInterval { get }

    @objc func onDisplaylink(displaylink: CADisplayLink) {
        // 打印时间戳
//        print("current:\(CACurrentMediaTime()) linkInfo.current:\(displaylink.timestamp) linkInfo.target\(displaylink.targetTimestamp)")
    }
    
    // 停止CADisplayLink
    func stopDisplaylink() {
        displaylink?.invalidate()
        displaylink = nil
    }
    
    
    init() {
        self.createDisplayLink()
    }
    
    
    public static let shared: DrawingContext = DrawingContext()
    
}

extension DrawingContext {
    /*
    video: 16 * 9
    video: 4 * 3
    video: 3 * 2

     3/3/5 45 720
     3/5 60 960
     3/5 75 1200
     3/5 80 1280

     
    width: 720、810、960、1080、1280、1440、1620、1920
           6    7   8    9     10    12   13    1
    960 * 540
    720 * 540
    810 * 540
    
    1280 * 720
    960 * 720
    1080 * 720

    1920 * 1080
    1440 * 1080
    1620 * 1080

    //72 54 108  180 * 4 180 * 3 180 * 6

    */

    
    public enum BoxSize {
        case preset720x540
        case preset960x540
        case preset960x720
        case preset1280x720
        case preset1280x1080
        case preset1440x1080
        case preset1920x1080
        var size: Size {
            switch self {
            case .preset720x540: return Size(width: 720, height: 540)
            case .preset960x540: return Size(width: 960, height: 540)
            case .preset960x720: return Size(width: 960, height: 720)
            case .preset1280x720: return Size(width: 1280, height: 720)
            case .preset1280x1080: return Size(width: 1280, height: 1080)
            case .preset1440x1080: return Size(width: 1440, height: 1080)
            case .preset1920x1080: return Size(width: 1920, height: 1080)
            }
        }
    }
    
    public class PageConfig {
        public let contextSize: Size
        public let bytesSize: Int
        public let bytesPerRow: Int
        public let countPerRow: Int32
        public let colorSpace: ColorSpace
        public let scale: CGFloat

        public init(boxSize: BoxSize, colorSpace: ColorSpace, scale: CGFloat) {
            let width = boxSize.size.width
            let height = width * 4
            self.scale = scale
            self.colorSpace = colorSpace
            let contextSize = Size(width: width, height: height)
            self.contextSize = contextSize
            let bytesPerRow = Int(width * colorSpace.bytesPerPixel)
            self.bytesPerRow = bytesPerRow
            self.countPerRow = width
            let bytesSize = bytesPerRow * Int(height)
            self.bytesSize = bytesSize
        }
    }
    
    public struct BoxConfig {
        public let boxSize: BoxSize
        public let page: PageConfig
        public let colorSpace: ColorSpace
        public let scale: CGFloat

//        public var backgroundColor: Color = Color(little32Argb: 0)
        
        public init(boxSize: BoxSize, scale: CGFloat = UIScreen.main.scale, colorSpace: ColorSpace = .little32Argb) {
            self.boxSize = boxSize
            self.scale = scale
            self.page = PageConfig(boxSize: boxSize, colorSpace: colorSpace, scale: scale)
            self.colorSpace = colorSpace
        }
    }
    

    public enum ColorSpace: UInt32 {
        //android 只有argb模式
        
        case little32Argb = 1
        case little16Xrgb = 2
        public static let deviceRgb: CGColorSpace = CGColorSpaceCreateDeviceRGB()

        public var bytesPerPixel: Int32 {
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
        
        public var space: CGColorSpace {
            return ColorSpace.deviceRgb
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
}



public struct Manager {
    public static let context: DrawingContext = DrawingContext()
    public static let specificKey: DispatchSpecificKey<DrawingContext> = DispatchSpecificKey<DrawingContext>()
    public static let queue: DispatchQueue = {
        let q = DispatchQueue(label: "d", qos: DispatchQoS.userInitiated, attributes: [], autoreleaseFrequency: .inherit, target: nil)
        q.setSpecific(key: Manager.specificKey, value: Manager.context)
        return q
    }()

    
    


}
