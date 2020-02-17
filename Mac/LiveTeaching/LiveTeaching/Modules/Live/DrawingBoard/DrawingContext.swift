//
//  DrawingContext.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/17.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingBitmap {
    var onSequenceUpdate: ((DrawingBitmap) -> Void)? = nil
    public private(set) var sequence: UInt64 = 0
    public let size: Size
    public let bufferSize: Int
    public let ptr: UnsafeMutableRawPointer
    public let bitmapContext: CGContext
    public let bitmapLayout: Drawing.BitmapLayout
    public var image: CGImage {
        let colorSpace = self.bitmapLayout.colorSpace
        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
        return image
    }
    
    public let dataProvider: CGDataProvider
    var timer: DispatchSourceTimer?
    public init(size: Size, bitmapLayout: Drawing.BitmapLayout) {
        assert(size.height > 0)
        assert(size.width <= bitmapLayout.countPerRow)
        self.bitmapLayout = bitmapLayout
        let bufferSize = bitmapLayout.bytesPerRow * Int(size.height)
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: CCGetCachelineSize())
        self.ptr = ptr
        self.bufferSize = bufferSize

        let colorSpace = bitmapLayout.colorSpace
        let bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.bitmapContext = bitmapContext
        self.size = size
        
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr, size: self.bufferSize) { (mptr, ptr, size) in
            print("de")
            }!
        self.dataProvider = dataProvider
        let timer: DispatchSourceTimer = DispatchSource.makeTimerSource(flags: DispatchSource.TimerFlags(), queue: DispatchQueue.main)
        timer.schedule(deadline: DispatchTime.now(), repeating: 2)
        self.timer = timer
        timer.setEventHandler {
            print("timer setmem")
            let size = Int(self.bufferSize/16)
            let random = Int(arc4random()) % (size / 2) / 4 * 4
            memset(self.ptr.advanced(by: random), Int32(arc4random()/2), size - random)
            self.setNeedsDisplay()
        }
        timer.resume()
        

        self.bitmapContext.setFillColor(UIColor.white.cgColor)
        self.bitmapContext.fill(CGRect.init(x: 0, y: 0, width: 100000, height: 2000000))
        let v: UnsafeMutablePointer<UInt16> = self.ptr.bindMemory(to: UInt16.self, capacity: self.bufferSize/2)
        for i in 0 ..< self.bufferSize/2 {
            print(v.advanced(by: i).pointee)
        }
    }
    
    public func setNeedsDisplay() {
        self.sequence += 1
        if let onSequenceUpdate = self.onSequenceUpdate {
            onSequenceUpdate(self)
        }
    }
  
    func makeImage(y: UInt32, height: UInt32) -> CGImage? {
        if height <= 0 {
            return nil
        }
        let colorSpace = self.bitmapLayout.colorSpace
        let bitmapInfo: UInt32 = colorSpace.bitmapInfo
        let offset = self.bitmapLayout.bytesPerRow * Int(y)
        let size = self.bitmapLayout.bytesPerRow * Int(height)
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr.advanced(by: offset), size: size) { (mptr, ptr, size) in
            print("dataProvider release")
            }!
        return CGImage(width: Int(self.size.width), height: Int(height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: self.bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
    }
    
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


//public class DrawingBitmap {
//    public struct DrawingBoardContext: Equatable {
//        public var identifier: String
//        public var sequence: UInt64
//        public var offset: Int32
//        public static func == (_ lhs: DrawingBoardContext, _ rhs: DrawingBoardContext) -> Bool {
//            return lhs.identifier == rhs.identifier && lhs.sequence == rhs.sequence && lhs.offset == rhs.offset
//        }
//    }
//
//    public private(set) var context: DrawingBoardContext? = nil
//
//    public let size: Size
//    private let mirroring: UnsafeMutableRawPointer
//    public let ptr: UnsafeMutableRawPointer
//    public let bitmapContext: CGContext
//    public let bitmapLayout: Drawing.BitmapLayout
//    public init(size: Size, bitmapLayout: Drawing.BitmapLayout) {
//        assert(size.height > 0)
//        assert(size.width <= bitmapLayout.countPerRow)
//        self.bitmapLayout = bitmapLayout
//        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
//        self.ptr = ptr
//
//        let mirroring = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
//        self.mirroring = mirroring
//
//        let colorSpace = bitmapLayout.colorSpace
//        self.bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
//        }, releaseInfo: nil)!
//        self.size = size
//    }
//}




public class DrawingContext {
    public let drawingSize: DrawingSize
    public let drawingView: DrawingView
    public let operationView: ViewContainer

    //最大值
    public let contentHeight: UInt32
    public let bitmapLayout: Drawing.BitmapLayout

    public init(drawingSize: DrawingSize, contentHeight: UInt32, bitmapLayout: Drawing.BitmapLayout) {
        self.drawingSize = drawingSize
        self.bitmapLayout = bitmapLayout
        self.contentHeight = contentHeight
        self.drawingView = DrawingView(drawingSize: drawingSize, contentHeight: contentHeight, bitmapLayout: bitmapLayout)
        self.operationView = ViewContainer(frame: CGRect())
    }
}
