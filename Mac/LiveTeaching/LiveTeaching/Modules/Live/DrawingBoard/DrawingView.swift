//
//  DrawingView.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
@_exported import CCFoundation
@_exported import Basic


extension Log {
    public static func description(of value: Double, decimal: Int, needsPaddingZero: Bool = true) -> String {
        return String(format: "%.0\(decimal)lf", value)
    }
    public static func description(of touch: UITouch, view: UIView?) -> String {
        return "<UITouch: \(touch.addressDescription)> timestamp:\(touch.timestamp) phase:\(touch.phase) tapCount:\(touch.tapCount) majorRadius:\(touch.majorRadius) majorRadiusTolerance:\(touch.majorRadiusTolerance) locationInView:\(touch.location(in: view))"
    }
    public static func description(of event: UIEvent?) -> String {
        if let event = event {
            return "<UIEvent: \(event.addressDescription)> timestamp:\(event.timestamp)"
        } else {
            return "<UIEvent: nil>"
        }
    }
    public static func logEvent(_ function: String = #function, view: UIView?, touches: Set<UITouch>, event: UIEvent?) {
        let touches = Array<UITouch>(touches).map{ (touch) -> String in
            return self.description(of: touch, view: view)
        }
        print("\(function) - touches(\(touches.count):\(touches)) event:\(self.description(of: event))")
    }
}

public struct TouchPoint: CustomStringConvertible, CustomDebugStringConvertible, Equatable {
    public var description: String {
        return "(x:\(Log.description(of: Double(self.location.x), decimal: 2)), y:\(Log.description(of: Double(self.location.y), decimal: 2)), timestamp:\(Log.description(of: Double(self.timestamp), decimal: 6))"
    }
    
    public var debugDescription: String {
        return "(x:\(Log.description(of: Double(self.location.x), decimal: 2)), y:\(Log.description(of: Double(self.location.y), decimal: 2)), timestamp:\(Log.description(of: Double(self.timestamp), decimal: 6))"
    }
    
    public var location: CGPoint
    public var timestamp: TimeInterval
    
    public init(location: CGPoint = CGPoint(), timestamp: TimeInterval = 0) {
        self.location = location
        self.timestamp = timestamp
    }
    public static func == (lhs: TouchPoint, rhs: TouchPoint) -> Bool {
        return lhs.location == rhs.location && lhs.timestamp == rhs.timestamp
    }
}



public class Shape {
//    public let path: CGPath

    public fileprivate(set) var paths: [Path] = []
    
    public init(begin: CGPoint) {
    }

    
}



public enum PathElement {
    case move(to: CGPoint)
    case close
    case line(to: CGPoint)
    
    //二元Bezier曲线
    case quadCurve(to: CGPoint, controlPoint: CGPoint)
    
    //三元Bezier曲线
    case curve(to: CGPoint, controlPoint1: CGPoint, controlPoint2: CGPoint)
    
    //圆弧
    case arc(center: CGPoint, radius: CGFloat, startAngle: CGFloat, endAngle: CGFloat, clockwise: Bool)
}

public class Path {
    public let path: UIBezierPath = UIBezierPath()
    public fileprivate(set) var points: [PathElement] = []
    
    public init() {
    }
    
    public func move(to point: CGPoint) {
        self.points.append(.move(to: point))
    }
    
    public func close() {
        self.points.append(.close)
    }
    
    public func addLine(to point: CGPoint) {
        self.points.append(.line(to: point))
    }
    
    public func addArc(_ center: CGPoint, radius: CGFloat, startAngle: CGFloat, endAngle: CGFloat, clockwise: Bool) {
        self.points.append(.arc(center: center, radius: radius, startAngle: startAngle, endAngle: endAngle, clockwise: clockwise))
    }
    
    public func addCurve(to point: CGPoint, controlPoint1: CGPoint, controlPoint2: CGPoint) {
        self.points.append(.curve(to: point, controlPoint1: controlPoint1, controlPoint2: controlPoint2))
    }

    public func addQuadCurve(to point: CGPoint, controlPoint: CGPoint) {
        self.points.append(.quadCurve(to: point, controlPoint: controlPoint))
    }
    
//    open class UIBezierPath : NSObject, NSCopying, NSSecureCoding {
//        public convenience init(rect: CGRect)
//
//        public convenience init(ovalIn rect: CGRect)
//
//        public convenience init(roundedRect rect: CGRect, cornerRadius: CGFloat) // rounds all corners with the same horizontal and vertical radius
//
//        public convenience init(roundedRect rect: CGRect, byRoundingCorners corners: UIRectCorner, cornerRadii: CGSize)
//
//        public convenience init(arcCenter center: CGPoint, radius: CGFloat, startAngle: CGFloat, endAngle: CGFloat, clockwise: Bool)
//
//        public convenience init(cgPath CGPath: CGPath)
//
//
//        public init()
//
//        public init?(coder: NSCoder)
//
//
//        // Returns an immutable CGPathRef which is only valid until the UIBezierPath is further mutated.
//        // Setting the path will create an immutable copy of the provided CGPathRef, so any further mutations on a provided CGMutablePathRef will be ignored.
//        open var cgPath: CGPath
//
//
//        // Path construction
//
//        open func move(to point: CGPoint)
//
//        open func addLine(to point: CGPoint)
//
//        open func addCurve(to endPoint: CGPoint, controlPoint1: CGPoint, controlPoint2: CGPoint)
//
//        open func addQuadCurve(to endPoint: CGPoint, controlPoint: CGPoint)
//
//        @available(iOS 4.0, *)
//        open func addArc(withCenter center: CGPoint, radius: CGFloat, startAngle: CGFloat, endAngle: CGFloat, clockwise: Bool)
//
//        open func close()
//
//
//        open func removeAllPoints()
//
//
//        // Appending paths
//
//        open func append(_ bezierPath: UIBezierPath)
//
//
//        // Modified paths
//
//        @available(iOS 6.0, *)
//        open func reversing() -> UIBezierPath
//
//
//        // Transforming paths
//
//        open func apply(_ transform: CGAffineTransform)
//
//
//        // Path info
//
//        open var isEmpty: Bool { get }
//
//        open var bounds: CGRect { get }
//
//        open var currentPoint: CGPoint { get }
//
//        open func contains(_ point: CGPoint) -> Bool
//
//
//        // Drawing properties
//
//        open var lineWidth: CGFloat
//
//        open var lineCapStyle: CGLineCap
//
//        open var lineJoinStyle: CGLineJoin
//
//        open var miterLimit: CGFloat // Used when lineJoinStyle is kCGLineJoinMiter
//
//        open var flatness: CGFloat
//
//        open var usesEvenOddFillRule: Bool // Default is NO. When YES, the even-odd fill rule is used for drawing, clipping, and hit testing.
//
//
//        open func setLineDash(_ pattern: UnsafePointer<CGFloat>?, count: Int, phase: CGFloat)
//
//        open func getLineDash(_ pattern: UnsafeMutablePointer<CGFloat>?, count: UnsafeMutablePointer<Int>?, phase: UnsafeMutablePointer<CGFloat>?)
//
//
//        // Path operations on the current graphics context
//
//        open func fill()
//
//        open func stroke()
//
//
//        // These methods do not affect the blend mode or alpha of the current graphics context
//        open func fill(with blendMode: CGBlendMode, alpha: CGFloat)
//
//        open func stroke(with blendMode: CGBlendMode, alpha: CGFloat)
//
//
//        open func addClip()
//    }

    
}


public class TouchEventHandler: NSObject {
    public var isEnabled: Bool = true
    override init() {
        super.init()
    }
}
public class TapEventHandler: TouchEventHandler {
    public var isPending: Bool = true
    
    override init() {
        super.init()
    }
}

public class TouchPointCollection {
    public private(set) var bounds: (CGPoint, CGPoint)

    public let identifier: Data
    public let timestamp: TimeInterval
    
    public private(set) var items: [TouchPoint]

    public init(identifier: Data, point: TouchPoint) {
        self.identifier = identifier
        self.timestamp = point.timestamp
        self.bounds = (point.location, point.location)
        self.items = [point]
    }
    
    public func append(_ point: TouchPoint) {
        self.items.append(point)
        let location = point.location
        var bounds = self.bounds
        if bounds.0.x > location.x {
            bounds.0.x = location.x
        }
        if bounds.0.y > location.y {
            bounds.0.y = location.y
        }
        if bounds.1.x < location.x {
            bounds.1.x = location.x
        }
        if bounds.1.y < location.y {
            bounds.1.y = location.y
        }
        self.bounds = bounds
    }
}


public class DrawingEventHandler: TouchEventHandler {
    public enum State: Int {
        case pending
        case moving
        case finished
    }

    fileprivate var recogning: (status: State, touch: UITouch, points: TouchPointCollection)? = nil

    private func pending(touch: UITouch, points: TouchPointCollection) {
        guard self.recogning == nil else {
            print("pending error")
            return
        }
        self.recogning = (.pending, touch, points)
    }
    public func becomeMoving() {
        guard let (status, touch, points) = self.recogning, status == .pending else {
            print("moving error")
            return
        }
        self.recogning = (.moving, touch, points)
    }
    public func finish() {
        guard let (status, touch, points) = self.recogning, status == .pending || status == .moving else {
            print("finish error")
            return
        }
        self.recogning = (.finished, touch, points)
    }
    public func reset() {
        self.recogning = nil
    }
    
    //开始识别
    public func beginDrawingRecognize(touch: UITouch, points: TouchPointCollection) {
        print("beginDrawingRecognize: \(points)")
        self.pending(touch: touch, points: points)
    }
    
    //识别失败
    public func didDrawingRecognizedFailed(_ points: TouchPointCollection) {
        print("didDrawingRecognizedFailed")
    }
    
    //识别成功
    public func didDrawingRecognizedSuccess(_ points: TouchPointCollection) {
        print("didDrawingRecognizedSuccess")
    }
    
    //识别成功 后 每有新的point会调用
    public func didDrawingRecognizedUpdate(_ points: TouchPointCollection, point: TouchPoint) {
        print("didDrawingRecognizedFinish")
    }
    
    //识别完成
    public func didDrawingRecognizedFinish(_ points: TouchPointCollection) {
        print("didDrawingRecognizedFinish")
    }
    
    public func append(point: TouchPoint) -> Bool {
        guard let (status, _, points) = self.recogning, status != .finished else {
            print("append failure")
            return false
        }
        if let last = points.items.last {
            if last == point {
                return false
            }
        }
        points.append(point)
        return true
    }
    
    override init() {
        super.init()
    }
    
//    public func pending(point: TouchPoint) {
//        self.append(point: point)
//    }
//    public func begin() {
//
//    }
//    public func move(to point: TouchPoint) {
//        self.append(point: point)
//
//    }
//    public func finish(point: TouchPoint) {
//        self.append(point: point)
//    }
}
public final class DrawingEventRecognizer: NSObject {
//长按  点击 move
    public override init() {
        super.init()
    }
    public var isEnabled: Bool = true

    public enum State : Int {
        case possible
        case recogning
        case recognizedDrawing
        case recognizedDoubleTouch
        case ended
        case faild
    }
    
//    public var isRecogning: Bool
    public private(set) var isRecogning: Bool = true

    public private(set) var state: State = .possible
    public var tapHandler: TapEventHandler?
    private let drawingHandler: DrawingEventHandler = DrawingEventHandler()
    private var touches: Set<UITouch> = []
    
    private var drawingTimeoutHandler: DispatchWorkItem?

    public func tapped(point: TouchPoint) {
        print("tapped: \(point)")
    }
    
    private func cancelDrawingTimeoutHandler() {
        if let handler = self.drawingTimeoutHandler {
            handler.cancel()
            self.drawingTimeoutHandler = nil
        }
    }
    
    public func touchesBegan(view: DrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        
        if self.state == .possible {
            if self.touches.isEmpty {
                self.cancelDrawingTimeoutHandler()
                if touches.count == 1 {
                    self.state = .recogning
                    print("recogning")
                    if let delegate = view.drawDelegate {
                        let touch = touches.first!
                        let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)

                        if let id = delegate.drawingViewShouldBeginRecognize(view, point: point) {
                            let collection = TouchPointCollection(identifier: id, point: point)
                            
                            let item = DispatchWorkItem(block: {[weak self] in
                                guard let strongSelf = self else {
                                    return
                                }
                                if let (status, _, points) = strongSelf.drawingHandler.recogning, status == .pending {
                                    strongSelf.state = .recognizedDrawing
                                    strongSelf.drawingHandler.becomeMoving()
                                    strongSelf.drawingTimeoutHandler = nil
                                    strongSelf.drawingHandler.didDrawingRecognizedSuccess(points)
                                }
                            })
                            self.drawingTimeoutHandler = item
                            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 0.086, execute: item)
                            self.drawingHandler.beginDrawingRecognize(touch: touch, points: collection)
                        }
                    }
                } else if touches.count == 2 {
                    self.state = .recognizedDoubleTouch
                    print("double touch")
                } else {
                    self.state = .faild
                }
            } else {
                fatalError("error")
            }
        } else if self.state == .recogning {
            let tmp = self.touches.intersection(touches)
            if tmp.count >= 3 {
                if let (status, _, points) = self.drawingHandler.recogning {
                    guard status == .pending else {
                        fatalError("error 32")
                    }
                    self.drawingHandler.reset()
                    self.cancelDrawingTimeoutHandler()
                    self.drawingHandler.didDrawingRecognizedFailed(points)
                }
                self.state = .faild
            }
        }
        self.touches = self.touches.intersection(touches)
        
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
    
    public func touchesMoved(view: DrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)

        if let (status, touch, points) = self.drawingHandler.recogning {
            if touches.contains(touch) {
                let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                if self.drawingHandler.append(point: point) {
                    if status == .moving {
                        self.drawingHandler.didDrawingRecognizedUpdate(points, point: point)
                    } else if status == .pending {
                        let bounds = points.bounds
                        assert(self.state == .recogning)
                        if bounds.1.x - bounds.0.x > 8 || bounds.1.y - bounds.0.y > 8 {
                            self.state = .recognizedDrawing
                            self.cancelDrawingTimeoutHandler()
                            self.drawingHandler.becomeMoving()
                            self.drawingHandler.didDrawingRecognizedSuccess(points)
                        }
                    }
                }
            }
        }

        
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
    
    private func _touchesEnded(view: DrawingView, touches: Set<UITouch>, with event: UIEvent?) {
        if let (status, touch, points) = self.drawingHandler.recogning {
            if touches.contains(touch) {
                let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                let appendResult = self.drawingHandler.append(point: point)
                if status == .moving {
                    if appendResult {
                        self.drawingHandler.didDrawingRecognizedUpdate(points, point: point)
                    }
                    self.drawingHandler.finish()
                    self.drawingHandler.didDrawingRecognizedFinish(points)
                    self.drawingHandler.reset()
                } else if status == .pending {
                    if let tapHandler = self.tapHandler, tapHandler.isEnabled {
                        self.drawingHandler.reset()
                        self.drawingHandler.didDrawingRecognizedFailed(points)
                        self.tapped(point: point)
                    } else {
                        if self.drawingHandler.append(point: point) {
                            self.state = .recognizedDrawing
                            self.cancelDrawingTimeoutHandler()
                            self.drawingHandler.becomeMoving()
                            self.drawingHandler.didDrawingRecognizedSuccess(points)
                            
                            self.drawingHandler.finish()
                            self.drawingHandler.didDrawingRecognizedFinish(points)
                            self.drawingHandler.reset()
                        } else {
                            print("_touchesEnded error")
                        }
                    }
                }
            }
        }
        
        for item in touches {
            self.touches.remove(item)
        }
        if self.touches.isEmpty {
            if self.state == .faild {
                self.state = .possible
            } else {
                self.state = .ended
                self.state = .possible
            }
        }

    }

    
    public func touchesEnded(view: DrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        self._touchesEnded(view: view, touches: touches, with: event)

    }
    public func touchesCancelled(view: DrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        self._touchesEnded(view: view, touches: touches, with: event)
    }
}


public protocol DrawingViewDrawDelegate: class {
    
    //返回一个identifer
    func drawingViewShouldBeginRecognize(_ view: DrawingView, point: TouchPoint) -> Data?
    
    //开始识别
    func drawingViewDidBeginRecognize(_ view: DrawingView, point: TouchPoint)
    
    //识别失败
    func drawingViewDidRecognizedFailed(_ view: DrawingView)
    
    //识别成功
    func drawingViewDidRecognizedSuccess(_ view: DrawingView)

    func drawingView(_ view: DrawingView, beginDraw points:[TouchPoint])
    func drawingView(_ view: DrawingView, draw to:TouchPoint)
    func drawingViewFinishDraw(_ view: DrawingView)
}

public class DrawingView: UIView {
    /*
     background
     content
     foreground
     */
    
    public let content: CATiledLayer
    
    //图形容器层
    public let shapeContainer: ViewContainer
    
    public let status: DrawingStatus
    public let bitmap: DrawingBitmap
    
    private let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()

    private var isDispatchEventToRecognizer: Bool = false
    
    internal var drawDelegate: DrawingViewDrawDelegate? = nil

    public init(status: DrawingStatus) {
        self.status = status
        var bounds = CGRect(origin: CGPoint(), size: status.drawingSize.cgSize)
        let content = CATiledLayer()
        content.contentsScale = UIScreen.main.scale
//        content.delegate = self
//        content.frame = bounds

        let shapeContainer: ViewContainer = ViewContainer(frame: bounds)
        var size = status.drawingSize.rawValue
        size.height = status.contentHeightLimit
        self.bitmap = DrawingBitmap(size: size, status: status)
        super.init(frame: bounds)
        self.clipsToBounds = true
        self.layer.masksToBounds = true
        self.layer.addSublayer(bitmap.layer)
        self._prepare()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func _prepare() {
        self.isMultipleTouchEnabled = true
    }
    public override func layoutSubviews() {
        super.layoutSubviews()
    }
    public func didUpdateStatus(from: DrawingStatus.Status, to status: DrawingStatus.Status) {
        var bounds = self.bounds
        bounds.origin.y = status.offset
        self.bounds = bounds
        self.bitmap.didUpdateStatus(from: from, to: status)
    }
    
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
        self.eventRecognizer.touchesBegan(view: self, touches: touches, with: event)
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesMoved(touches, with: event)
            return
        }
        self.eventRecognizer.touchesMoved(view: self, touches: touches, with: event)
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesEnded(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesEnded(view: self, touches: touches, with: event)
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesCancelled(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesCancelled(view: self, touches: touches, with: event)
    }
}

