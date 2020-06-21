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
        
        print("draw(_ layer  \(bounds)")
    }
    
}


public class DrawingBitmap: TiledBitmap {
    var onSequenceUpdate: ((DrawingBitmap) -> Void)? = nil
    public private(set) var sequence: UInt64 = 0
    public let sliceSize: Size
    
    public let bitmapContext: CGContext

//    public var image: CGImage {
//        let bitmapLayout = self.status.bitmapLayout
//        let colorSpace = bitmapLayout.colorSpace
//        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//        return image
//    }
    public private(set) var slices: [DrawingBitmapSliceLayer] = []
    
    public let dataProvider: CGDataProvider
    public var contentHeight: UInt32 {
        didSet(old) {
            if old != self.contentHeight {
                let scale = UIScreen.main.scale
                self.layer.frame = CGRect(x: 0, y: 0, width: CGFloat(self.size.width) / scale, height: CGFloat(self.contentHeight) / scale)
                
                if let last = self.slices.last {
                    if last.size != self.sliceSize {
                        last.removeFromSuperlayer()
                        self.slices.removeLast()
                    }
                }
                let count = Int(self.contentHeight / self.sliceSize.height)
                if count > self.slices.count {
                    for idx in self.slices.count ..< count {
                        let y = UInt32(idx) * self.sliceSize.height
                        let dataProvider: CGDataProvider = self.makeDataProvider(y: y, height: self.sliceSize.height)
                        let layer = DrawingBitmapSliceLayer(y: y, size: self.sliceSize, dataProvider: dataProvider)
                        self.layer.addSublayer(layer)
                        self.slices.append(layer)
                    }
                } else if count < self.slices.count {
                    for item in self.slices.suffix(self.slices.count - count) {
                        item.removeFromSuperlayer()
                    }
                    self.slices.removeLast(self.slices.count - count)
                }
                let remain = self.contentHeight % self.sliceSize.height
                if remain > 0 {
                    var size = self.sliceSize
                    size.height = remain
                    let y = UInt32(self.slices.count) * self.sliceSize.height
                    let dataProvider: CGDataProvider = self.makeDataProvider(y: y, height: remain)
                    let layer = DrawingBitmapSliceLayer(y: y, size: size, dataProvider: dataProvider)
                    self.layer.addSublayer(layer)
                    self.slices.append(layer)
                }
                
                
            }
        }
    }
    var timer: DispatchSourceTimer?
//    let observer = DrawingStatus.Observer()

    public let status: DrawingStatus
    public init(size: Size, contentHeight: UInt32 = 0, status: DrawingStatus) {
        let bitmapLayout = status.bitmapLayout
        assert(size.height > 0)
        assert(size.width <= bitmapLayout.countPerRow)
        var sliceSize: Size = size
        sliceSize.height = size.width / 2
        self.sliceSize = sliceSize
        self.status = status
        self.contentHeight = contentHeight
        let bufferSize = bitmapLayout.bytesPerRow * Int(size.height)
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: CCGetCachelineSize())
        self.ptr = ptr
        self.bufferSize = bufferSize
        
        let colorSpace = bitmapLayout.colorSpace
        let bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        let layer: BitmapLayer = BitmapLayer()
        layer.zPosition = -100
        let scale = UIScreen.main.scale
        layer.frame = CGRect(x: 0, y: 0, width: CGFloat(size.width) / scale, height: CGFloat(contentHeight) / scale)
        
        let dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr, size: self.bufferSize) { (mptr, ptr, size) in
            print("de")
            }!
        self.dataProvider = dataProvider
        
//        let image = CGImage(width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: colorSpace.bitmapInfo), provider: self.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)!
//        self.image = image
        
//        let timer: DispatchSourceTimer = DispatchSource.makeTimerSource(flags: DispatchSource.TimerFlags(), queue: DispatchQueue.main)
//        timer.schedule(deadline: DispatchTime.now(), repeating: 2)
//        self.timer = timer
//        timer.setEventHandler {
//            print("timer setmem")
//            let size = Int(self.bufferSize)
//            let random = Int(arc4random()) % 10 * self.status.bitmapLayout.bytesPerRow * 64
//            memset(self.ptr.advanced(by: random), Int32(arc4random()/2), self.status.bitmapLayout.bytesPerRow * 64)
//            self.displayContent()
//        }
//        timer.resume()
//
//        print("self.bufferSize: \(self.bufferSize)")
//
//
//        let timeBegin = CFAbsoluteTimeGetCurrent();
//        self.bitmapContext.setFillColor(UIColor.white.cgColor)
//        self.bitmapContext.fill(CGRect.init(x: 0, y: 0, width: 100000, height: 2000000))
//        let timeEnd = CFAbsoluteTimeGetCurrent();
//        print(timeEnd - timeBegin)
//
//        CCMemorySetUInt64(self.ptr, 0x7f_ff_7f_80_70_ff_7f_ff, self.bufferSize/8)
//        let timeEnd2 = CFAbsoluteTimeGetCurrent();
//        print(timeEnd2 - timeEnd)

        super.init(size: size, visibleFrame: <#Rect#>)
//        status.addObserver(self.observer)
//        self.observer.didUpdate = {[weak self] (_ observer: DrawingStatus.Observer, _ context: DrawingStatus, _ from: DrawingStatus.Status, _ to: DrawingStatus.Status) in
//            guard let strongSelf = self else {
//                return
//            }
//            strongSelf.contentHeight = to.contentHeight
//
//
//
//            let sliceSize = strongSelf.status.drawingSize.cgSize
//            let top = to.offset > 0 ? to.offset : 0
//            let bottom = to.offset + sliceSize.height
//
//            if top < strongSelf.layer.frame.origin.y || bottom > strongSelf.layer.frame.origin.y + strongSelf.layer.frame.size.height {
//                let t = top - (sliceSize.width - sliceSize.height) / 2
//                let y: UInt32
//                let scale = UIScreen.main.scale
//                if t < 0 {
//                    y = 0
//                } else {
//                    let tmp = Int(t * scale - 1)
//                    y = tmp > 0 ? UInt32(tmp) : 0
//                }
//
//                strongSelf.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
//                strongSelf.layer.cgImage = strongSelf.makeImage(y: y, height: strongSelf.status.drawingSize.rawValue.height)
//            }
//
//
//        }
    }
    deinit {
//        self.status.removeObserver(self.observer)
    }
    
    public func didUpdateStatus(from: DrawingStatus.Status, to: DrawingStatus.Status) {
        self.contentHeight = to.contentHeight
        
        let sliceSize = self.status.drawingSize.cgSize
        let top = to.offset > 0 ? to.offset : 0
        let bottom = to.offset + sliceSize.height
        
        if top < self.layer.frame.origin.y || bottom > self.layer.frame.origin.y + self.layer.frame.size.height {
            let t = top - (sliceSize.width - sliceSize.height) / 2
            let y: UInt32
            let scale = UIScreen.main.scale
            if t < 0 {
                y = 0
            } else {
                let tmp = Int(t * scale - 1)
                y = tmp > 0 ? UInt32(tmp) : 0
            }
            
            self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
            self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)
        }
    }
    
    public func displayContent() {
        let y: UInt32 = 0
        let scale = UIScreen.main.scale
        let sliceSize = self.status.drawingSize.cgSize

        self.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: sliceSize.width, height: sliceSize.width)
        self.layer.cgImage = self.makeImage(y: y, height: self.status.drawingSize.rawValue.height)

//        if self.sequence > 5 {
//            for slice in self.slices {
//                let image = slice.cgImage
//                slice.cgImage = image
//                if let aimage = slice.cgImage  {
//
//                }
//                slice.setNeedsDisplay()
//            }
//            self.layer.setNeedsDisplay()
//            self.layer.superlayer?.setNeedsDisplay()
//            return
//        }
//        let bitmapLayout = self.status.bitmapLayout
//        let colorSpace = bitmapLayout.colorSpace
//        let bitmapInfo: UInt32 = colorSpace.bitmapInfo
//        for (idx, slice) in self.slices.enumerated() {
//            if idx < 16 {
//                slice.cgImage = CGImage(width: Int(self.size.width), height: Int(slice.size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bitsPerPixel: Int(colorSpace.bitsPerPixel), bytesPerRow: bitmapLayout.bytesPerRow, space: Drawing.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: slice.dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
//            } else {
//                slice.contents = nil
//            }
//        }
//        self.layer.setNeedsDisplay()
//
//        self.sequence += 1
    }
  
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

