//
//  DrawingView.swift
//  ScrollView
//
//  Created by vector on 2019/9/2.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

/*
 video: 16 * 9
 video: 4 * 3
 video: 3 * 2

 width: 720、810、960、1080、1280、1440、1620、1920
        6    7   8    9     10    12   13    15
 
 
 
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



public final class DrawCache<KeyType : AnyObject & Hashable, ObjectType : AnyObject> {
    fileprivate class CacheEntry<KeyType : AnyObject, ObjectType : AnyObject> {
        var key: KeyType
        var value: ObjectType
        var cost: Int
        var prevByCost: CacheEntry?
        var nextByCost: CacheEntry?
        init(key: KeyType, value: ObjectType, cost: Int) {
            self.key = key
            self.value = value
            self.cost = cost
        }
    }
    
    private var _entries = Dictionary<KeyType, CacheEntry<KeyType, ObjectType>>()
    private var _totalCost = 0
    private var _head: CacheEntry<KeyType, ObjectType>?
    
    public var name: String = ""
    public var totalCostLimit: Int = 0 // limits are imprecise/not strict

    public init() {}
    
    public func object(forKey key: KeyType) -> ObjectType? {
        var object: ObjectType?
        if let entry = _entries[key] {
            object = entry.value
        }
        return object
    }
    
    public func setObject(_ obj: ObjectType, forKey key: KeyType) {
        setObject(obj, forKey: key, cost: 0)
    }
    
    private func remove(_ entry: CacheEntry<KeyType, ObjectType>) {
        let oldPrev = entry.prevByCost
        let oldNext = entry.nextByCost
        
        oldPrev?.nextByCost = oldNext
        oldNext?.prevByCost = oldPrev
        
        if entry === _head {
            _head = oldNext
        }
    }
   
    private func insert(_ entry: CacheEntry<KeyType, ObjectType>) {
        guard var currentElement = _head else {
            // The cache is empty
            entry.prevByCost = nil
            entry.nextByCost = nil
            
            _head = entry
            return
        }
        
        guard entry.cost > currentElement.cost else {
            // Insert entry at the head
            entry.prevByCost = nil
            entry.nextByCost = currentElement
            currentElement.prevByCost = entry
            
            _head = entry
            return
        }
        
        while let nextByCost = currentElement.nextByCost, nextByCost.cost < entry.cost {
            currentElement = nextByCost
        }
        
        // Insert entry between currentElement and nextElement
        let nextElement = currentElement.nextByCost
        
        currentElement.nextByCost = entry
        entry.prevByCost = currentElement
        
        entry.nextByCost = nextElement
        nextElement?.prevByCost = entry
    }
    
    public func setObject(_ obj: ObjectType, forKey key: KeyType, cost g: Int) {
        let g = max(g, 0)
        let keyRef = key
                
        let costDiff: Int
        
        if let entry = _entries[keyRef] {
            costDiff = g - entry.cost
            entry.cost = g
            
            entry.value = obj
            
            if costDiff != 0 {
                remove(entry)
                insert(entry)
            }
        } else {
            let entry = CacheEntry(key: key, value: obj, cost: g)
            _entries[keyRef] = entry
            insert(entry)
            
            costDiff = g
        }
        
        _totalCost += costDiff
        
        var purgeAmount = (totalCostLimit > 0) ? (_totalCost - totalCostLimit) : 0
                
        while purgeAmount > 0 {
            if let entry = _head {
                _totalCost -= entry.cost
                purgeAmount -= entry.cost
                
                let key = entry.key
                
                autoreleasepool { () -> Void in
                    remove(entry) // _head will be changed to next entry in remove(_:)
                    _entries[key] = nil
                }
            } else {
                break
            }
        }
    }
    
    public func removeObjectStrongRefrence(forKey key: KeyType) {
        let keyRef = key
        autoreleasepool { () -> Void in
            if let entry = _entries.removeValue(forKey: keyRef) {
                _totalCost -= entry.cost
                remove(entry)
            }
        }
    }
    
    public func removeAllObjectStrongRefrences() {
        _entries.removeAll()
        autoreleasepool { () -> Void in
            while let currentElement = _head {
                let nextElement = currentElement.nextByCost
                
                currentElement.prevByCost = nil
                currentElement.nextByCost = nil
                
                _head = nextElement
            }
        }
        _totalCost = 0
    }
}



public protocol TouchesEventHandleable: class {
    func handleTouchesBegan(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
    func handleTouchesMoved(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
    func handleTouchesEnded(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
    func handleTouchesCancelled(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool
}

public class DrawingEventHandler: TouchesEventHandleable {
    public let event: UIEvent
    public let id: UInt32

    public init(event: UIEvent, id: UInt32) {
        self.event = event
        self.id = id
    }
    
    public func handleTouchesBegan(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
        guard event == self.event else {
            return false
        }
        
        return true
    }
    public func handleTouchesMoved(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
        guard event == self.event else {
            return false
        }
        
        return true
    }
    public func handleTouchesEnded(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
        guard event == self.event else {
            return false
        }
        
        return true
    }
    public func handleTouchesCancelled(_ view: UIView, touches: Set<UITouch>, with event: UIEvent?) -> Bool {
        guard event == self.event else {
            return false
        }
        
        return true
    }

    
}

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



public class DrawingContext {

    public enum BoxSize {
        case preset960x540
        case preset810x540
        case preset1280x720
        case preset1080x720
        case preset1920x1080
        case preset1620x1080
        var size: Size {
            switch self {
            case .preset960x540: return Size(width: 960, height: 540)
            case .preset810x540: return Size(width: 810, height: 540)
            case .preset1280x720: return Size(width: 1280, height: 720)
            case .preset1080x720: return Size(width: 1080, height: 720)
            case .preset1920x1080: return Size(width: 1920, height: 1080)
            case .preset1620x1080: return Size(width: 1620, height: 1080)
            }
        }
    }
    
    public struct Config {
        public let boxSize: BoxSize
        public let page: BitMapPage.Config
        public var backgroundColor: Color = Color(little32Argb: 0)
        
        public init(boxSize: BoxSize) {
            self.boxSize = boxSize
            self.page = BitMapPage.Config(boxSize: boxSize)
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
    
    public class Item {
        public let origin: Point
        public let size: Size
        public init(origin: Point, size: Size) {
            self.origin = origin
            self.size = size
        }
        
        
    }
    
    public class BitMapPage {
        public class Config {
            public static let chunkSize: Int32 = 0x10
            
            public static func sizeAlign(size: Int32) -> Int32 {
                return (size + Config.chunkSize - 1) / Config.chunkSize * Config.chunkSize
            }
            
            public let contextSize: Size
            public let bytesSize: Int
            public let bytesPerRow: Int
            public let colorSpace: ColorSpace
                        
            public init(boxSize: BoxSize) {
                let width = Config.sizeAlign(size: boxSize.size.width)
                let height = Config.sizeAlign(size: (width + 1) / 2)
                let colorSpace: ColorSpace = .little32Argb
                self.colorSpace = colorSpace
                let contextSize = Size(width: width, height: height)
                self.contextSize = contextSize
                let bytesPerRow = Int(width * colorSpace.bytesPerPixel)
                self.bytesPerRow = bytesPerRow
                let bytesSize = bytesPerRow * Int(height)
                self.bytesSize = bytesSize
            }
        }
        
        public let config: Config

        public let mainPtr: UnsafeMutableRawPointer
        public let duplicatePtr: UnsafeMutableRawPointer
        public let context: CGContext
        public let dataProvider: CGDataProvider

        public let layer: CALayer
        
        
        public init(config: Config) {
            self.config = config
            self.layer = CALayer()
            let mainPtr = UnsafeMutableRawPointer.allocate(byteCount: config.bytesSize, alignment: 128)
            self.mainPtr = mainPtr
            self.duplicatePtr = UnsafeMutableRawPointer.allocate(byteCount: config.bytesSize, alignment: 128)
            let colorSpace = config.colorSpace

            self.context = CGContext(data: mainPtr, width: Int(config.contextSize.width), height: Int(config.contextSize.height), bitsPerComponent: colorSpace.bitsPerComponent, bytesPerRow: config.bytesPerRow, space: DrawingContext.ColorSpace.deviceRgb, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
            }, releaseInfo: nil)!
            
            self.dataProvider = CGDataProvider(dataInfo: nil, data: self.mainPtr, size: self.config.bytesSize) { (mptr, ptr, size) in
                print("de")
                }!
        
        }
        
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
    }
    
    
    private var drawIndex: UInt32 = 0
    private var items: [Shape] = []

    
    public let layer: CALayer
    public let backgroundImageLayer: CALayer
    public let size: CGSize
    
    public init(config: Config) {
        let layer = CALayer()
        self.layer = layer
        
        let scale = UIScreen.main.scale
        let size = CGSize(width: CGFloat(config.boxSize.size.width) / scale, height: CGFloat(config.boxSize.size.height) / scale)

        let frame = CGRect(x: 0, y: 0, width: size.width, height: size.height)
        layer.frame = frame
        
        
        self.backgroundImageLayer = CALayer()
        self.size = size
    }

    
    public func shouldBegin(event: UIEvent) -> TouchesEventHandleable? {
        let handler = DrawingEventHandler(event: event, id: self.drawIndex)
        self.drawIndex += 1
        return handler
    }
    
    
}




public protocol Stroke: class {
    
}

open class DrawingBox: UIStackView {

    
}

open class DrawingView: UIView {
    fileprivate var context: DrawingContext? = nil {
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
    
    public var drawingHandler: TouchesEventHandleable?
    
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let e = event else {
            self.drawingHandler = nil
//            super.touchesBegan(touches, with: event)
            print("touchesBegan error: \(touches)")
            return
        }
        guard let context = self.context else {
            self.drawingHandler = nil
//            super.touchesBegan(touches, with: event)
            print("touchesBegan context is nil")
            return
        }
        guard let handler = context.shouldBegin(event: e) else {
            self.drawingHandler = nil
//            super.touchesBegan(touches, with: event)
            print("touchesBegan shouldBegin return nil")
            return
        }
        self.drawingHandler = handler
        if !handler.handleTouchesBegan(self, touches: touches, with: event) {
            self.logTouch(touches, with: e)
        }
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let e = event else {
//            super.touchesMoved(touches, with: event)
            print("touchesMoved error: \(touches)")
            return
        }
        guard let handler = self.drawingHandler else {
//            super.touchesMoved(touches, with: event)
            print("touchesMoved drawingItem is nil")
            return
        }
        if !handler.handleTouchesMoved(self, touches: touches, with: event) {
            self.logTouch(touches, with: e)
        }
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let e = event else {
//            super.touchesEnded(touches, with: event)
            print("touchesEnded error: \(touches)")
            return
        }
        guard let handler = self.drawingHandler else {
//            super.touchesEnded(touches, with: event)
            print("touchesEnded drawingItem is nil")
            return
        }
        if !handler.handleTouchesEnded(self, touches: touches, with: event) {
            self.logTouch(touches, with: e)
        }
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let e = event else {
//            super.touchesCancelled(touches, with: event)
            print("touchesCancelled error: \(touches)")
            return
        }
        guard let handler = self.drawingHandler else {
//            super.touchesCancelled(touches, with: event)
            print("touchesCancelled drawingItem is nil")
            return
        }
        if !handler.handleTouchesCancelled(self, touches: touches, with: event) {
            self.logTouch(touches, with: e)
        }
    }
    
    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
    }
    
    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
        
        print("\(function) \(item())")
    }
    

    
    
    
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


