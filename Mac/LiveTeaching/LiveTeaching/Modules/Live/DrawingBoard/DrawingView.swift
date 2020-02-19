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

public class BaseDrawEventHandler: NSObject {
    public var isEnabled: Bool = true
    
    override init() {
        super.init()
    }
}
public class TapEventHandler: BaseDrawEventHandler {
    public var isPending: Bool = true

    
    
    override init() {
        super.init()
    }
}
public class DrawingTouchPointArray: BaseDrawEventHandler {
    public private(set) var points: [TouchPoint] = []
    public private(set) var bounds: (CGPoint, CGPoint)?
    
    fileprivate var touch: UITouch? = nil

    public enum State : Int {
        case possible
        case pending
        case moving
        case finished
    }
    
    public var state: State = .possible {
        didSet {
            if self.state == .possible {
                self.points = []
                self.bounds = nil
                self.touch = nil
            }
        }
    }
    
    public func append(point: TouchPoint) -> Bool {
        guard self.state != .possible && self.state != .finished else {
            print("DrawHandler.append error")
            return false
        }
        if let last = self.points.last {
            if last == point {
                return false
            }
        }
        self.points.append(point)
        let location = point.location
        if var bounds = self.bounds {
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
        } else {
            self.bounds = (location, location)
        }
        return true
    }
    public func removeAll() {
        self.points.removeAll()
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
//
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
    private let drawingTouchPoints: DrawingTouchPointArray = DrawingTouchPointArray()
    private var touches: Set<UITouch> = []
    
    private var drawingTimeoutHandler: DispatchWorkItem?

    public func beginDraw(points: [TouchPoint]) {
        print("beginDraw: \(points) current:\(self.drawingTouchPoints.points)")
    }
    public func draw(to point: TouchPoint) {
        print("drawTo: \(point) current:\(self.drawingTouchPoints.points)")
    }
    public func finishDraw() {
        print("finishDraw current:\(self.drawingTouchPoints.points)")
    }
    
    public func tapped(point: TouchPoint) {
        print("tapped: \(point)")
    }
    
    private func cancelDrawingTimeoutHandler() {
        if let handler = self.drawingTimeoutHandler {
            handler.cancel()
            self.drawingTimeoutHandler = nil
        }
    }
    
    public func touchesBegan(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        
        if self.state == .possible {
            if self.touches.isEmpty {
                self.cancelDrawingTimeoutHandler()
                if touches.count == 1 {
                    self.state = .recogning
                    print("recogning")
                    
                    let touch = touches.first!
                    let item = DispatchWorkItem(block: {[weak self] in
                        guard let `self` = self else {
                            return
                        }
                        if self.drawingTouchPoints.state == .pending {
                            self.state = .recognizedDrawing
                            self.drawingTouchPoints.state = .moving
                            self.drawingTimeoutHandler = nil
                            self.beginDraw(points: self.drawingTouchPoints.points)
                        }
                    })
                    self.drawingTimeoutHandler = item
                    DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 0.086, execute: item)
                    self.drawingTouchPoints.state = .pending
                    let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                    self.drawingTouchPoints.touch = touch
                    let _ = self.drawingTouchPoints.append(point: point)
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
                self.drawingTouchPoints.state = .possible
                self.cancelDrawingTimeoutHandler()
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
    
    public func touchesMoved(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)

        if let touch = self.drawingTouchPoints.touch {
            if touches.contains(touch) {
                let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                if self.drawingTouchPoints.append(point: point) {
                    if self.drawingTouchPoints.state == .moving {
                        self.draw(to: point)
                    } else if self.drawingTouchPoints.state == .pending {
                        let bounds = self.drawingTouchPoints.bounds!
                        assert(self.state == .recogning)
                        if bounds.1.x - bounds.0.x > 2 || bounds.1.y - bounds.0.y > 2 {
                            self.state = .recognizedDrawing
                            self.drawingTouchPoints.state = .moving
                            self.cancelDrawingTimeoutHandler()
                            self.beginDraw(points: self.drawingTouchPoints.points)
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
    public func touchesEnded(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)

        if let touch = self.drawingTouchPoints.touch {
            if touches.contains(touch) {
                let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                let appendResult = self.drawingTouchPoints.append(point: point)
                if self.drawingTouchPoints.state == .moving {
                    if appendResult {
                        self.draw(to: point)
                    }
                    self.drawingTouchPoints.state = .finished
                    self.finishDraw()
                    self.drawingTouchPoints.state = .possible
                } else if self.drawingTouchPoints.state == .pending {
                    self.drawingTouchPoints.state = .possible
                    self.tapped(point: point)
                }
            }
        }
        
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
    public func touchesCancelled(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)

        if let touch = self.drawingTouchPoints.touch {
            if touches.contains(touch) {
                let point = TouchPoint(location: touch.location(in: view), timestamp: touch.timestamp)
                let appendResult = self.drawingTouchPoints.append(point: point)

                if self.drawingTouchPoints.state == .moving {
                    if appendResult {
                        self.draw(to: point)
                    }
                    self.drawingTouchPoints.state = .finished
                    self.finishDraw()
                    self.drawingTouchPoints.state = .possible
                } else if self.drawingTouchPoints.state == .pending {
                    self.drawingTouchPoints.state = .possible
                }
            }
        }
        
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
}


public protocol DrawingViewDrawDelegate: class {
    
    //开始识别
    func drawingViewDidBeginRecognize(_ view: DrawingView)
    
    //识别失败
    func drawingViewDidRecognizedFailed(_ view: DrawingView)
    
    //识别成功
    func drawingViewDidRecognizedSuccess(_ view: DrawingView)

    func drawingView(_ view: DrawingView, beginDraw points:[TouchPoint])
    func drawingView(_ view: DrawingView, draw to:TouchPoint)
    func drawingViewFinishDraw(_ view: DrawingView)
}

public class DrawingView: UIView {
    public let status: DrawingStatus
    public let bitmap: DrawingBitmap
    
    private let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()
    
    private var isDispatchEventToRecognizer: Bool = false
        
    internal var drawDelegate: DrawingViewDrawDelegate? = nil

    public init(status: DrawingStatus) {
        self.status = status
        var size = status.drawingSize.rawValue
        size.height = status.contentHeightLimit
        self.bitmap = DrawingBitmap(size: size, status: status)
        super.init(frame: CGRect(origin: CGPoint(), size: status.drawingSize.cgSize))
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
        self.bitmap.contentHeight = status.contentHeight
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

