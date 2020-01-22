//
//  Controller.swift
//  ScrollView
//
//  Created by vector on 2020/1/15.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


//public class SliceCollection {
//    private class Row {
//        public let slices: [Slice]
//        public init(slices: [Slice]) {
//            self.slices = slices
//        }
//    }
//    private let rows: [Row]
//
//    private let slicesSize: C2DSize
//    public init?(size: C2DSize, slicesSize: C2DSize) {
//        if slicesSize.width < 0 || slicesSize.height < 0 {
//            return nil
//        }
//        if size.width < 0 || size.height < 0 {
//            return nil
//        }
//        self.slicesSize = slicesSize
//
//        let numberOfSlicePerRow: Int32
//        if slicesSize.width == 1 {
//            numberOfSlicePerRow = size.width
//        } else {
//            numberOfSlicePerRow = (size.width + slicesSize.width - 1) / slicesSize.width
//        }
//        let rowCount: Int32
//        if slicesSize.height == 1 {
//            rowCount = size.height
//        } else {
//            rowCount = (size.height + slicesSize.height - 1) / slicesSize.height
//        }
//        var rows: [Row] = []
//        for rowIndex in 0 ..< rowCount {
//            let y = slicesSize.height * rowIndex
//            var slices: [Slice] = []
//            slices.reserveCapacity(Int(numberOfSlicePerRow))
//            for sliceIndex in 0 ..< numberOfSlicePerRow {
//                let frame = C2DRectMake(sliceIndex * slicesSize.width, y, slicesSize.width, slicesSize.height)
//                slices.append(Slice(frame: frame))
//            }
//            rows.append(Row(slices: slices))
//        }
//        self.rows = rows
//    }
//}

//public class SliceCollection {
//    private let rows: [Slice]
//
//    private let sliceHeight: Int32
//    public init?(sliceCount: Int32, sliceHeight: Int32) {
//        if sliceHeight <= 0 {
//            return nil
//        }
//        if sliceCount < 0 {
//            return nil
//        }
//
//        var slices: [Slice] = []
//        if sliceCount > 0 {
//            for index in 0 ..< sliceCount {
//                let y = sliceHeight * index
//                slices.append(Slice(y: y))
//            }
//        }
//        self.rows = slices
//    }
//}


public class DrawingBoard {
    public struct DrawingBoardContext: Equatable {
        public var identifier: String
        public var sequence: UInt64
        public var offset: Int32
        public static func == (_ lhs: DrawingBoardContext, _ rhs: DrawingBoardContext) -> Bool {
            return lhs.identifier == rhs.identifier && lhs.sequence == rhs.sequence && lhs.offset == rhs.offset
        }
    }
    
    public private(set) var context: DrawingBoardContext? = nil
    public let size: Size
    
    private let mirroring: UnsafeMutableRawPointer
    public let ptr: UnsafeMutableRawPointer
    public let bitmapContext: CGContext
    public let bitmapLayout: BitmapLayout
    public init(size: Size, bitmapLayout: BitmapLayout) {
        assert(size.height > 0)
        assert(size.width <= bitmapLayout.countPerRow)
        self.bitmapLayout = bitmapLayout
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
        self.ptr = ptr
        
        let mirroring = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
        self.mirroring = mirroring

        let colorSpace = bitmapLayout.colorSpace
        self.bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.size = size
    }
    
    deinit {
        self.ptr.deallocate()
    }
    
    private func clear() {
        
    }
    
    public func map(rect: CGRect, scale: CGFloat) -> Rect? {
        var frame = rect.standardized
        frame = rect.inset(by: UIEdgeInsets(top: 2, left: 2, bottom: 2, right: 2))
        var bounds = Rect(x: CCSInt32(frame.origin.x * scale), y: CCSInt32(frame.origin.y * scale), width: CCUInt32(frame.size.width * scale), height: CCUInt32(frame.size.height * scale))

//        bounds = C2DRectEnlargeFrame(<#T##from: C2DRect##C2DRect#>, <#T##to: C2DRect##C2DRect#>)


        if Int64(bounds.origin.x) + Int64(bounds.size.width) < 0 {
            return nil
        }
        if Int64(bounds.origin.y) + Int64(bounds.size.height) < 0 {
            return nil
        }
        
        if bounds.origin.x < 0 {
            bounds.size.width -= CCUInt32(rect.origin.x * -1)
            bounds.origin.x = 0
        }
        if rect.origin.y < 0 {
            bounds.size.height -= CCUInt32(rect.origin.y * -1)
            bounds.origin.y = 0
        }

        let alignment = UInt32(CCGetCachelineSize()) / self.bitmapLayout.colorSpace.bytesPerPixel
        
        if alignment > 1 {
            let xOffset = bounds.origin.x % Int32(alignment)
            bounds.origin.x -= xOffset
            bounds.size.width += UInt32(xOffset)

            let widthOffset = bounds.size.width % UInt32(alignment)
            if widthOffset > 0 {
                bounds.size.width += (UInt32(alignment) - widthOffset)
            }
        }
        return bounds
    }

    
//    public func map(rect: CGRect, config: DrawingContext.BoxConfig, originY: Int32, frames: inout [Int32: C2DBytesRange]) -> C2DRect? {
//        var frame = rect.standardized
//        frame = rect.inset(by: UIEdgeInsets(top: 2, left: 2, bottom: 2, right: 2))
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

    
    public func begin(controller: DrawingBoardController, offset: Int32) throws {
        let context = DrawingBoardContext(identifier: controller.identifier, sequence: controller.sequence, offset: offset)
        if self.context != context {
            self.context = context
            self.clear()
        }
    }
    
    public func beginDrawing() {}
    
    public func finish() {
        
    }
}

public class SliceContext: NSObject {
    public let info: SliceInfo
    public let fileContent: FileContent
    public let fileData: FileData

    public init(info: SliceInfo, fileData: FileData) {
//        let filehandle: FileHandle = FileHandle.init(forReadingAtPath: "")!
//        let data = filehandle.readData(ofLength: 9)
//        data.withUnsafeBytes { (ptr) -> Void in
//            CCMemoryCopy(self.ptr, ptr, )
//        }
        self.info = info
        self.fileData = fileData
        self.fileContent = fileData.beginContentAccess()
    }
    deinit {
        self.fileData.endContentAccess()
    }
}

public class SliceInfo: NSObject {
    public let ownerIdentifier: String
    public let y: Int32
    public let size: Size
    public let bitmapLayout: BitmapLayout
    public init(ownerIdentifier: String, y: Int32, size: Size, bitmapLayout: BitmapLayout) {
        self.ownerIdentifier = ownerIdentifier
        self.y = y
        self.size = size
        self.bitmapLayout = bitmapLayout
    }
}
public class Slice: NSObject {
    public enum Status {
        case normal
        case displaying(context: SliceContext)
        case prepared(context: SliceContext)
    }
    
    public let fileData: FileData
    public let info: SliceInfo
    public var status: Status
    public private(set) var context: SliceContext? = nil
    public let layer: CALayer
    
    public init(ownerIdentifier: String, y: Int32, size: Size, status: Status, bitmapLayout: BitmapLayout) {
        let path = ownerIdentifier.appending("/\(y)")
        self.info = SliceInfo(ownerIdentifier: ownerIdentifier, y: y, size: size, bitmapLayout: bitmapLayout)
        self.status = status
        self.fileData = FileManager.shared.beginAccessFileData(path: path)
        self.layer = CALayer()
    }
    
    deinit {
        FileManager.shared.endAccess(fileData: self.fileData)
    }
}

public class DrawingBoardController {
    public static let zPosition: CGFloat = 100

    public let identifier: String
    public private(set) var sequence: UInt64 = 0

    private let items: [Slice]
    public let size: Size
    public let sliceHeight: UInt32

    public private(set) var offset: Int32
    public private(set) var visibleSlices: [Slice]

    public let bitmapLayout: BitmapLayout
    
    public let layer: CALayer
    public let scale: CGFloat
    
    public init(identifier: String, size: Size, offset: Int32, bitmapLayout: BitmapLayout, scale: CGFloat, layer: CALayer) {
        let sliceHeight: UInt32 = 256
        let sliceCount: UInt32 = (size.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: size.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                let slice = Slice(ownerIdentifier: identifier, y: y, size: sliceSize, status: .normal, bitmapLayout: bitmapLayout)
                slice.layer.zPosition = DrawingBoardController.zPosition
                slice.layer.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: CGFloat(size.width) / scale, height: CGFloat(sliceHeight) / scale)
                layer.addSublayer(slice.layer)
                slices.append(slice)
            }
        }
        self.items = slices
        self.sliceHeight = sliceHeight
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.size = size
        self.offset = offset
        self.visibleSlices = []
        self.layer = layer
        self.scale = scale
    }
    
    public func beginDrawing(borad: DrawingBoard) throws {
        try borad.begin(controller: self, offset: self.offset)
    }
    
    public func pointBuffer(y: Int32) -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer.allocate(byteCount: 10, alignment: CCGetCachelineSize())
    }
}


public class TapEventHandler: NSObject {
    open var isEnabled: Bool = false

    override init() {
        super.init()
    }
}
public class PanEventHandler: NSObject {
    open var isEnabled: Bool = false

    override init() {
        super.init()
    }
}
public final class DrawingEventRecognizer: NSObject {
//长按  点击 move
    public override init() {
        super.init()
    }
    public var isEnabled: Bool = false

    public enum State : Int {
        case possible
        case pan
        case pan2
        case recognized
        case ended
        case cancelled
        case failed
//        public static var recognized: UIGestureRecognizer.State { get }
    }
    
//    public var recogning: Bool
    
    
    public var tapHandler: TapEventHandler?
    public var panHandler: PanEventHandler?

    public func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
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
    }
    public func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
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
    }
    public func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
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
    }
    public func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
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
    }

}


public class DrawingContainer: UIStackView {
    public let drawingBoard: DrawingBoardView
    
    public override init(frame: CGRect) {
        self.drawingBoard = DrawingBoardView(frame: CGRect())
        super.init(frame: frame)
        self.addSubview(self.drawingBoard)
        self.drawingBoard.frame = self.bounds
    }
    public required init(coder: NSCoder) {
        self.drawingBoard = DrawingBoardView(frame: CGRect())
        super.init(coder: coder)
        self.addSubview(self.drawingBoard)
        self.drawingBoard.frame = self.bounds
    }
}

public class DrawingBoardView: UIView {
    public let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()
    
    private var isDispatchEventToRecognizer: Bool = false
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        if self.eventRecognizer.isEnabled {
            self.isDispatchEventToRecognizer = true
        } else {
            self.isDispatchEventToRecognizer = false
        }
        guard self.isDispatchEventToRecognizer else {
            super.touchesBegan(touches, with: event)
            return
        }
        self.eventRecognizer.touchesBegan(touches, with: event)
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesMoved(touches, with: event)
            return
        }
        self.eventRecognizer.touchesMoved(touches, with: event)
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesEnded(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesEnded(touches, with: event)
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesCancelled(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesCancelled(touches, with: event)
    }
}


public class DrawingController {
    public let drawingBoardController: DrawingBoardController
    
    public let identifier: String
    public let container: DrawingContainer
    public private(set) var sequence: UInt64 = 0

    private let items: [Slice]
    public let bounds: Rect
    public let sliceHeight: UInt32

    public private(set) var offset: Int32
    public private(set) var visibleSlices: [Slice]
    public let layer: CALayer

    public let bitmapLayout: BitmapLayout
    public init(identifier: String, bounds: Rect, offset: Int32, bitmapLayout: BitmapLayout, scale: CGFloat) {
        let layer: CALayer = CALayer()
        layer.frame = CGRect(x: 0, y: 0, width: CGFloat(bounds.size.width) / scale, height: CGFloat(bounds.size.height) / scale)
        layer.masksToBounds = true
        
        let container: DrawingContainer = DrawingContainer(frame: layer.frame)
        container.layer.addSublayer(layer)
        
        self.drawingBoardController = DrawingBoardController(identifier: identifier.appending("/drawing"), size: bounds.size, offset: offset, bitmapLayout: bitmapLayout, scale: scale, layer: layer)
        
        let sliceHeight: UInt32 = 256
        let sliceCount: UInt32 = (bounds.size.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: bounds.size.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                slices.append(Slice(ownerIdentifier: identifier, y: y, size: sliceSize, status: .normal, bitmapLayout: bitmapLayout))
            }
        }
        self.items = slices
        self.sliceHeight = sliceHeight
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.bounds = bounds
        self.offset = offset
        self.visibleSlices = []
        self.layer = layer
        self.container = container
                
    }
    

    



    
    
}
