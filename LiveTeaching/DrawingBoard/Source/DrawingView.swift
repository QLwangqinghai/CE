//
//  DrawingView.swift
//  ScrollView
//
//  Created by vector on 2019/9/2.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit




public struct TouchPoint {
    public var location: CGPoint
    public var velocity: CGPoint
    public var time: TimeInterval
    
    public init(location: CGPoint = CGPoint(), velocity: CGPoint = CGPoint(), time: TimeInterval = 0) {
        self.location = location
        self.velocity = velocity
        self.time = time
    }
}



public protocol Drawable: class {
    func getFrame() -> CGRect
    
    
    
}


public protocol DrawingEventHandle: class {
    func begin(_ point: TouchPoint)
    func change(_ point: TouchPoint)
    func finish(_ point: TouchPoint)
}

public class DrawingEventHandler: DrawingEventHandle {
    public let id: UInt32
    public let path: UIBezierPath = UIBezierPath()

    private var previousPoint: TouchPoint = TouchPoint()
    
    public init(id: UInt32) {
        self.id = id
    }
    private func midpoint(_ p0: TouchPoint, _ p1: TouchPoint) -> CGPoint {
        return CGPoint(x: (p0.location.x + p1.location.x) / 2.0, y: (p0.location.y + p1.location.y) / 2.0)
    }
    
    public func begin(_ point: TouchPoint) {
        self.previousPoint = point
        self.path.move(to: point.location)
    }
    public func change(_ point: TouchPoint) {
        let midPoint = self.midpoint(self.previousPoint, point);
        self.path.addQuadCurve(to: midPoint, controlPoint: previousPoint.location)
        self.previousPoint = point;
    }
    public func finish(_ point: TouchPoint) {
        let midPoint = self.midpoint(self.previousPoint, point);
        self.path.addQuadCurve(to: midPoint, controlPoint: previousPoint.location)
        self.previousPoint = point;
    }
}

//public class Pan {
//    public let panGestureRecognizer: UIPanGestureRecognizer = UIPanGestureRecognizer()
//    public override init() {
//        super.init()
//        self.panGestureRecognizer.addTarget(self, action: #selector(DrawingLayer.handlePan))
//    }
//
//    @objc private func handlePan(recognizer: UIPanGestureRecognizer) {
//        guard recognizer == self.panGestureRecognizer else {
//            return
//        }
//        let location = recognizer.location(in: recognizer.view)
//        let point = TouchPoint(location: location, velocity: recognizer.velocity(in: recognizer.view), time: CACurrentMediaTime())
//        if recognizer.state == .began {
//            self.begin(point)
//        } else if recognizer.state == .changed {
//            self.change(point)
//        } else {
//            if recognizer.state == .cancelled || recognizer.state == .ended {
//                self.finish(point)
//            }
//        }
//        self.setNeedsDisplay()
//    }
//}


public final class DrawingLayer: CALayer {
    
    public let id: UInt32
    public let path: UIBezierPath = UIBezierPath()

    private var previousPoint: TouchPoint = TouchPoint()
    
    public init(id: UInt32) {
        self.id = id
        super.init()
    }
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func midpoint(_ p0: TouchPoint, _ p1: TouchPoint) -> CGPoint {
        return CGPoint(x: (p0.location.x + p1.location.x) / 2.0, y: (p0.location.y + p1.location.y) / 2.0)
    }
    
    public func begin(_ point: TouchPoint) {
        self.previousPoint = point
        self.path.move(to: point.location)
    }
    public func change(_ point: TouchPoint) {
        let midPoint = self.midpoint(self.previousPoint, point);
        self.path.addQuadCurve(to: midPoint, controlPoint: previousPoint.location)
        self.previousPoint = point;
    }
    public func finish(_ point: TouchPoint) {
        let midPoint = self.midpoint(self.previousPoint, point);
        self.path.addQuadCurve(to: midPoint, controlPoint: previousPoint.location)
        self.previousPoint = point;
    }
}


//public final class GestureEventHandler: NSObject {
//    public let panGestureRecognizer: UIPanGestureRecognizer = UIPanGestureRecognizer()
//
//    override init() {
//        super.init()
//        self.panGestureRecognizer.addTarget(self, action: #selector(GestureEventHandler.handlePan))
//    }
//
//    public var handler: TouchEventHandleable?
//
//    @objc private func handlePan(recognizer: UIPanGestureRecognizer) {
//        guard recognizer == self.panGestureRecognizer else {
//            return
//        }
//        let location = recognizer.location(in: recognizer.view)
//        let point = TouchPoint(location: location, velocity: recognizer.velocity(in: recognizer.view), time: CACurrentMediaTime())
//
//        guard let handler = self.handler else {
//            return
//        }
//
//        if recognizer.state == .began {
//            handler.begin(point)
//        } else if recognizer.state == .changed {
//            handler.change(point)
//        } else {
//            if recognizer.state == .cancelled || recognizer.state == .ended {
//                handler.finish(point)
//            }
//        }
//    }
//}



//public protocol TouchesEventHandleable: class {
//    func handleTouchesBegan(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
//    func handleTouchesMoved(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
//    func handleTouchesEnded(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
//    func handleTouchesCancelled(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
//}
//
//public class DrawingEventHandler: TouchesEventHandleable {
//    public let event: UIEvent
//    public let id: UInt32
//
//    public init(event: UIEvent, id: UInt32) {
//        self.event = event
//        self.id = id
//    }
//
//    public func handleTouchesBegan(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
//        guard event == self.event else {
//            return false
//        }
//
//        return true
//    }
//    public func handleTouchesMoved(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
//        guard event == self.event else {
//            return false
//        }
//
//        return true
//    }
//    public func handleTouchesEnded(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
//        guard event == self.event else {
//            return false
//        }
//
//        return true
//    }
//    public func handleTouchesCancelled(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
//        guard event == self.event else {
//            return false
//        }
//
//        return true
//    }
//}

open class Shape {
    public let id: UInt32

    public init(id: UInt32) {
        self.id = id
    }
    
//    open func begin(_ point: DrawingPoint) {
//        
//    }
//    open func move(_ point: DrawingPoint) {
//        
//    }
//    open func end(_ point: DrawingPoint) {
//        
//    }
//    open func cancel(_ point: DrawingPoint) {
//        
//    }

}



public class BoxBounds {
    public private(set) var y: Int32
    public let height: Int32
    public init(y: Int32, height: Int32) {
        self.y = y
        self.height = height
    }
    
    
    
}


public class ScheduleContext {

    
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
    
    public class Item {
        public let origin: Point
        public let size: Size
        public init(origin: Point, size: Size) {
            self.origin = origin
            self.size = size
        }
    }
    
//    public class BitMapPage {
//        public class BitMapLineBuffer {
//            public var buffer: [CCRange] = []
//
//            public func append(_ range: C2DBytesRange) {
//                self.buffer.append(range)
//            }
//        }
//
//
//        public let config: DrawingContext.PageConfig
//
//        public let ptr: UnsafeMutableRawPointer
//        public let bitMapPage: C2DBitMapPage_s
//        public let context: CGContext
//        public let dataProvider: CGDataProvider
//
//        public let layer: CALayer
//        public let originY: Int32
//
//
//        public init(config: DrawingContext.PageConfig, originY: Int32) {
//            self.config = config
//            self.layer = CALayer()
//            let ptr = UnsafeMutableRawPointer.allocate(byteCount: config.bytesSize, alignment: 128)
//            self.ptr = ptr
//
//            let colorSpace = config.colorSpace
//
//
//            let bitMapPage: C2DBitMapPage_s = C2DBitMapPage_s(frame: C2DRect(origin: Point(x: 0, y: originY), size: config.contextSize), countPerRow: config.countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
//            self.bitMapPage = bitMapPage
//
//
//            self.context = CGContext(data: ptr, width: Int(config.contextSize.width), height: Int(config.contextSize.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: config.bytesPerRow, space: config.colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
//            }, releaseInfo: nil)!
//
//            self.dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr, size: self.config.bytesSize) { (mptr, ptr, size) in
//                print("de")
//                }!
//        }
//
//
//        public func draw(_ item: Drawable) {
//
//        }
//
//
//        func makeImage(x: UInt32, y: UInt32, width: UInt32, height: UInt32) -> CGImage? {
//            let right = UInt64(x) + UInt64(width)
//            let bottom = UInt64(y) + UInt64(height)
//            if right > UInt64(self.config.width) {
//                return nil
//            }
//            if bottom > UInt64(self.config.height) {
//                return nil
//            }
//            let config = self.config
//            let colorSpace = config.colorSpace
//            let bitmapInfo: UInt32 = colorSpace.bitmapInfo
//
//            let offset = config.bytesPerRow * Int(y) + Int(colorSpace.bytesPerPixel * x)
//            let dataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr.advanced(by: offset), size: self.config.bufferSize - offset) { (mptr, ptr, size) in
//                print("de")
//                }!
//            return CGImage(width: Int(width), height: Int(height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: config.bytesPerRow, space: DrawingContext.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
//        }
//    }
//
//    public class Storage {
//        public private(set) var buffer: (UnsafeMutableRawPointer, Int)
//        public var store: C2DBitMapStore_s
//        public let bytesPerPixel: Int32
//        public let contextSize: Size
//        public let bytesPerRow: Int32
//        public let countPerRow: Int32
//        public let colorSpace: DrawingContext.ColorSpace
//
//        public init(boxSize: DrawingContext.BoxSize, colorSpace: DrawingContext.ColorSpace, frame: C2DRect) {
//            let width = boxSize.size.width
//            let height = width
//            self.colorSpace = colorSpace
//            let contextSize = Size(width: width, height: height)
//            self.contextSize = contextSize
//            self.bytesPerPixel = colorSpace.bytesPerPixel
//            let bytesPerRow = width * colorSpace.bytesPerPixel
//            self.bytesPerRow = bytesPerRow
//            self.countPerRow = width
//
//            let bufferSize = Int(bytesPerRow * height)
//            let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: 128)
//            self.buffer = (ptr, bufferSize)
//
//            let countPerRow: Int32 = bytesPerPixel * frame.size.width
//            let store: C2DBitMapStore_s = C2DBitMapStore_s(frame: frame, validFrame: C2DRect(), offset: 0, bytesPerRow: countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
//            self.store = store
//        }
//
//        deinit {
//            self.buffer.0.deallocate()
//        }
//    }
//
    private var drawIndex: UInt32 = 0
    private var items: [Shape] = []

    
    public let layer: CALayer
    public let backgroundImageLayer: CALayer
    public let size: CGSize
    
    public let drawingLayer: CALayer

    public private(set) var pendingItem: Drawable? = nil
    
    public func updatePendingItem(_ pendingItem: Drawable?) {
        //revert

        
        self.pendingItem = pendingItem
        
        
        //save
        
    }
    
    
    
//    private var store: Storage? = nil {
//        Storage.init(frame: <#T##C2DRect#>, bytesPerPixel: <#T##Int32#>)
//        let bitMapPage: C2DBitMapPage_s = C2DBitMapPage_s(frame: C2DRect(origin: Point(x: 0, y: originY), size: config.contextSize), countPerRow: config.countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
//
//    } ()
//    public let boxBounds: BoxBounds

    public init(config: DrawingContext.BoxConfig) {
        let layer = CALayer()
        self.layer = layer
        
        let drawingLayer = CALayer()
        self.drawingLayer = drawingLayer
        
        let scale = UIScreen.main.scale
        let size = CGSize(width: CGFloat(config.boxSize.size.width) / scale, height: CGFloat(config.boxSize.size.height) / scale)

        let frame = CGRect(x: 0, y: 0, width: size.width, height: size.height)
        layer.frame = frame
        
        self.backgroundImageLayer = CALayer()
        self.size = size
    }

//    public func shouldBegin(event: UIEvent) -> TouchesEventHandleable? {
//        let handler = DrawingEventHandler(event: event, id: self.drawIndex)
//        self.drawIndex += 1
//        return handler
//    }
//
    
}




//
//public class ScheduleContext {
//
//    public class Item {
//        public let origin: Point
//        public let size: Size
//        public init(origin: Point, size: Size) {
//            self.origin = origin
//            self.size = size
//        }
//    }
//
//    public class BitMapPage {
//        public class BitMapLineBuffer {
//            public var buffer: [C2DBytesRange] = []
//
//            public func append(_ range: C2DBytesRange) {
//                self.buffer.append(range)
//            }
//        }
//
//
//        public let config: DrawingContext.PageConfig
//
//        public let ptr: UnsafeMutableRawPointer
//        public let bitMapPage: C2DBitMapPage_s
//        public let context: CGContext
//        public let dataProvider: CGDataProvider
//
//        public let layer: CALayer
//        public let originY: Int32
//
//
//        public init(config: DrawingContext.PageConfig, originY: Int32) {
//            self.config = config
//            self.layer = CALayer()
//            let ptr = UnsafeMutableRawPointer.allocate(byteCount: config.bytesSize, alignment: 128)
//            self.ptr = ptr
//
//            let colorSpace = config.colorSpace
//
//
//            let bitMapPage: C2DBitMapPage_s = C2DBitMapPage_s(frame: C2DRect(origin: Point(x: 0, y: originY), size: config.contextSize), countPerRow: config.countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
//            self.bitMapPage = bitMapPage
//
//
//            self.context = CGContext(data: ptr, width: Int(config.contextSize.width), height: Int(config.contextSize.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: config.bytesPerRow, space: config.colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
//            }, releaseInfo: nil)!
//
//            self.dataProvider = CGDataProvider(dataInfo: nil, data: self.ptr, size: self.config.bytesSize) { (mptr, ptr, size) in
//                print("de")
//                }!
//        }
//
//
//        public func draw(_ item: Drawable) {
//
//        }
//
//
////        func makeImage(x: UInt32, y: UInt32, width: UInt32, height: UInt32) -> CGImage? {
////            let right = UInt64(x) + UInt64(width)
////            let bottom = UInt64(y) + UInt64(height)
////            if right > UInt64(self.config.width) {
////                return nil
////            }
////            if bottom > UInt64(self.config.height) {
////                return nil
////            }
////            let config = self.config
////            let colorSpace = config.colorSpace
////            let bitmapInfo: UInt32 = colorSpace.bitmapInfo
////
////            let offset = config.bytesPerRow * Int(y) + Int(colorSpace.bytesPerPixel * x)
////            let dataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr.advanced(by: offset), size: self.config.bufferSize - offset) { (mptr, ptr, size) in
////                print("de")
////                }!
////            return CGImage(width: Int(width), height: Int(height), bitsPerComponent: colorSpace.bitsPerComponent, bitsPerPixel: colorSpace.bitsPerPixel, bytesPerRow: config.bytesPerRow, space: DrawingContext.ColorSpace.deviceRgb, bitmapInfo:CGBitmapInfo(rawValue: bitmapInfo), provider: dataProvider, decode: nil, shouldInterpolate: true, intent: CGColorRenderingIntent.defaultIntent)
////        }
//    }
//
//    public class Storage {
//        public private(set) var buffer: (UnsafeMutableRawPointer, Int)
//        public var store: C2DBitMapStore_s
//        public let bytesPerPixel: Int32
//        public let contextSize: Size
//        public let bytesPerRow: Int32
//        public let countPerRow: Int32
//        public let colorSpace: DrawingContext.ColorSpace
//
//        public init(boxSize: DrawingContext.BoxSize, colorSpace: DrawingContext.ColorSpace, frame: C2DRect) {
//            let width = boxSize.size.width
//            let height = width
//            self.colorSpace = colorSpace
//            let contextSize = Size(width: width, height: height)
//            self.contextSize = contextSize
//            self.bytesPerPixel = colorSpace.bytesPerPixel
//            let bytesPerRow = width * colorSpace.bytesPerPixel
//            self.bytesPerRow = bytesPerRow
//            self.countPerRow = width
//
//            let bufferSize = Int(bytesPerRow * height)
//            let ptr = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: 128)
//            self.buffer = (ptr, bufferSize)
//
//            let countPerRow: Int32 = bytesPerPixel * frame.size.width
//            let store: C2DBitMapStore_s = C2DBitMapStore_s(frame: frame, validFrame: C2DRect(), offset: 0, bytesPerRow: countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
//            self.store = store
//        }
//
//        deinit {
//            self.buffer.0.deallocate()
//        }
//    }
//
//    private var drawIndex: UInt32 = 0
//    private var items: [Shape] = []
//
//
//    public let layer: CALayer
//    public let backgroundImageLayer: CALayer
//    public let size: CGSize
//
//    public let drawingLayer: CALayer
//
//    public private(set) var pendingItem: Drawable? = nil
//
//    public func updatePendingItem(_ pendingItem: Drawable?) {
//        //revert
//
//
//        self.pendingItem = pendingItem
//
//
//        //save
//
//    }
//
//
//
////    private var store: Storage? = nil {
////        Storage.init(frame: <#T##C2DRect#>, bytesPerPixel: <#T##Int32#>)
////        let bitMapPage: C2DBitMapPage_s = C2DBitMapPage_s(frame: C2DRect(origin: Point(x: 0, y: originY), size: config.contextSize), countPerRow: config.countPerRow, bytesPerPixel: colorSpace.bytesPerPixel, content: ptr)
////
////    } ()
////    public let boxBounds: BoxBounds
//
//    public init(config: DrawingContext.BoxConfig) {
//        let layer = CALayer()
//        self.layer = layer
//
//        let drawingLayer = CALayer()
//        self.drawingLayer = drawingLayer
//
//        let scale = UIScreen.main.scale
//        let size = CGSize(width: CGFloat(config.boxSize.size.width) / scale, height: CGFloat(config.boxSize.size.height) / scale)
//
//        let frame = CGRect(x: 0, y: 0, width: size.width, height: size.height)
//        layer.frame = frame
//
//        self.backgroundImageLayer = CALayer()
//        self.size = size
//    }
//
////    public func shouldBegin(event: UIEvent) -> TouchesEventHandleable? {
////        let handler = DrawingEventHandler(event: event, id: self.drawIndex)
////        self.drawIndex += 1
////        return handler
////    }
////
//
//}
//


public protocol Stroke: class {
    
}


open class DrawingBox: UIStackView {

    
}

open class DrawingView: UIView {
    fileprivate var context: ScheduleContext? = nil {
        didSet(oldValue) {
            if let old = oldValue {
                old.layer.removeFromSuperlayer()
            }
            if let new = self.context {
                self.layer.addSublayer(new.layer)
            }
        }
    }
    
//    public override init(frame: CGRect) {
//        super.init(frame: frame)
//        let layer: CALayer =
//    }
    
//    public var handler: GestureEventHandler = GestureEventHandler()
    
    
//    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
//            self.drawingHandler = nil
////            super.touchesBegan(touches, with: event)
//            print("touchesBegan error: \(touches)")
//            return
//        }
//        guard let context = self.context else {
//            self.drawingHandler = nil
////            super.touchesBegan(touches, with: event)
//            print("touchesBegan context is nil")
//            return
//        }
//        guard let handler = context.shouldBegin(event: e) else {
//            self.drawingHandler = nil
////            super.touchesBegan(touches, with: event)
//            print("touchesBegan shouldBegin return nil")
//            return
//        }
//        self.drawingHandler = handler
//        if !handler.handleTouchesBegan(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
//    }
//    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
////            super.touchesMoved(touches, with: event)
//            print("touchesMoved error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesMoved(touches, with: event)
//            print("touchesMoved drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesMoved(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
//    }
//    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
////            super.touchesEnded(touches, with: event)
//            print("touchesEnded error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesEnded(touches, with: event)
//            print("touchesEnded drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesEnded(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
//    }
//    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
////            super.touchesCancelled(touches, with: event)
//            print("touchesCancelled error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesCancelled(touches, with: event)
//            print("touchesCancelled drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesCancelled(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
//    }
//
    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
    }
    
    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
        
        print("\(function) \(item())")
    }
    

//    public func map(frame: CGRect, config: DrawingContext.BoxConfig, originY: Int32, frames: inout [Int32: C2DBytesRange]) -> C2DRect? {
//        var tmp = frame.standardized
//
//        let scale = config.scale
//        var bounds = C2DRectMake(Int32(tmp.origin.x * scale) - 1, Int32(tmp.origin.y * scale) - 1, Int32(tmp.size.width * scale) + 2, Int32(tmp.size.height * scale) + 2)
//        let m = C2DRectMake(0, 0, config.contextSize.width, INT32_MAX)
//
//        if !C2DRectIntersect(m, bounds, &bounds) {
//            return nil
//        }
//
////        bounds = C2DRectEnlargeFrame(<#T##from: C2DRect##C2DRect#>, <#T##to: C2DRect##C2DRect#>)
//        if bounds.origin.x % 2 != 0 {
//            bounds.origin.x -= 1
//            bounds.size.width += 1
//        }
//        if bounds.origin.y % 2 != 0 {
//            bounds.origin.y -= 1
//            bounds.size.height += 1
//        }
//        if bounds.size.width % 2 != 0 {
//            bounds.size.width += 1
//        }
//        if bounds.size.height % 2 != 0 {
//            bounds.size.height += 1
//        }
//
//        var rect = C2DRect()
//        if rect.origin.x < 0 {
//            rect.size.width += rect.origin.x
//            rect.origin.x = 0
//        }
//        if rect.origin.y < 0 {
//            rect.origin.y = 0
//        }
//
//        if config.colorSpace.bytesPerPixel < 8 {
//            let mask = (8 / config.colorSpace.bytesPerPixel)
//            let offset = x % mask
//            if offset != 0 {
//                rect.origin.x -= offset
//                rect.size.width += offset
//            }
//        }
//        return rect
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
    
    func resetContent() {

    }
    
}

open class DrawingScrollView: UIScrollView {
    public var contentView: DrawingView
    var array: [CGImage?] = []
    
    public var context: ScheduleContext? = nil {
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
    
//    public var drawingItem: (item: TouchesEventHandleable, event: UIEvent)?
//
//
//    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
//            super.touchesBegan(touches, with: event)
//            return
//        }
//
//        self.logTouch(touches, with: e)
//    }
//    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
//            super.touchesMoved(touches, with: event)
//            return
//        }
//        self.logTouch(touches, with: e)
//    }
//    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
//            super.touchesEnded(touches, with: event)
//            return
//        }
//        self.logTouch(touches, with: e)
//    }
//    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let e = event else {
//            super.touchesCancelled(touches, with: event)
//            return
//        }
//        self.logTouch(touches, with: e)
//    }
//
//    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
////        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
//    }
//
//    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
////        print("\(file) :\(line): \(function) \(item())")
//
//        print("\\(function) \(item())")
//    }
//
    func resetContent() {
        
    }
}

