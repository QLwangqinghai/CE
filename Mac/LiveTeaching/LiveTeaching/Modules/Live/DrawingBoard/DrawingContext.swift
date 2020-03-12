//
//  DrawingContainer.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/17.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
import CCFoundation


public class DrawingBitmapSliceLayer: CALayer {
    public let y: UInt32
    public let size: Size
    public var cgImage: CGImage? {
        didSet {
            self.contents = self.cgImage
        }
    }

    internal let dataProvider: CGDataProvider
    public init(y: UInt32, size: Size, dataProvider: CGDataProvider) {
        self.y = y
        self.size = size
        self.dataProvider = dataProvider
        super.init()
        let scale = UIScreen.main.scale
        self.frame = CGRect(x: 0, y: CGFloat(y) / scale, width: CGFloat(self.size.width) / scale, height: CGFloat(self.size.height) / scale)
    }
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    open override func draw(in ctx: CGContext) {
        guard let cgImage = self.cgImage else {
            return
        }
        print("draw: \(self.y)")
        ctx.draw(cgImage, in: self.bounds)
    }
}

public class DrawingBitmapLayer: CALayer {
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

public class DrawingStatus {
    public struct Status: Equatable {
        public var offset: CGFloat = 0
        public var contentHeight: UInt32 = 0
        public static func == (_ lhs: Status, _ rhs: Status) -> Bool {
            return lhs.offset == rhs.offset && lhs.contentHeight == rhs.contentHeight
        }
    }
    public class Observer {
        public typealias ObserveClosure = (_ observer: Observer, _ context: DrawingStatus, _ from: Status, _ to: Status) -> Void
        public var didUpdate: ObserveClosure
        public init() {
            self.didUpdate = {(_, _, _, _) in
            }
        }
    }
    
    public let drawingSize: DrawingSize
    //最大值
    public let contentHeightLimit: UInt32
    public let bitmapLayout: Drawing.BitmapLayout
    public private(set) var status: Status = Status()
    private var observers: [Observer] = []
    
    public init(drawingSize: DrawingSize, contentHeightLimit: UInt32, bitmapLayout: Drawing.BitmapLayout) {
        self.drawingSize = drawingSize
        self.bitmapLayout = bitmapLayout
        self.contentHeightLimit = contentHeightLimit
    }
    
    public func addObserver(_ observer: Observer) {
        self.observers.append(observer)
    }
    public func removeObserver(_ observer: Observer) {
        self.observers.removeAll(where: { (item) -> Bool in
            return item === observer
        })
    }
    
    internal func updateStatus(_ status: Status, closure: (_ status: DrawingStatus, _ old:Status, _ new: Status)-> Void) {
        let old = self.status
        if old != status {
            self.status = status
            let observers = self.observers
            closure(self, old, status)
            for observer in observers {
                observer.didUpdate(observer, self, old, status)
            }
        }
    }
}


public class DrawingContainer: StackView {
    public let container: StackView

    public let drawingView: DrawingView
    public let operationView: ViewContainer
    public private(set) var status: DrawingStatus
    
    public init(frame: CGRect, drawingSize: DrawingSize, contentHeightLimit: UInt32, bitmapLayout: Drawing.BitmapLayout) {
        let container: StackView = StackView(frame: CGRect(origin: CGPoint(), size: drawingSize.cgSize))
        let status = DrawingStatus(drawingSize: drawingSize, contentHeightLimit: contentHeightLimit, bitmapLayout: bitmapLayout)
        self.drawingView = DrawingView(status: status)
        self.operationView = ViewContainer(frame: CGRect())
        self.status = status
        self.container = container
        super.init(frame: frame)
        self.addSubview(container)

        container.addSubview(self.drawingView)
        self.addSubview(self.operationView)
        self.layoutSubviews()
    }
    
    required init(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
    public func updateStatus(_ status: DrawingStatus.Status) {
        self.status.updateStatus(status) { (status, old, new) in
            self.drawingView.didUpdateStatus(from: old, to: new)
        }
//        let old = self.status.status
//        if old != status {
//            self.status.status = status
//            let observers = self.observers
//            for observer in observers {
//                observer.willUpdate(observer, self, old, status)
//            }
//            self.status = status
//            self.drawingView.didUpdateStatus(from: old, to: status)
//            for observer in observers.reversed() {
//                observer.didUpdate(observer, self, old, status)
//            }
//        }
    }
    
    open override func layout() {
        super.layout()
        let bounds = self.bounds
        let drawingSize = self.status.drawingSize.cgSize
        self.container.bounds = CGRect(origin: CGPoint(), size: drawingSize)
        self.container.transform = CGAffineTransform.identity.scaledBy(x: bounds.size.width / drawingSize.width, y: bounds.size.height / drawingSize.height)
        self.container.center = CGPoint(x: bounds.size.width/2, y: bounds.size.height/2)
        self.operationView.frame = CGRect(origin: CGPoint(), size: bounds.size)
    }
}
