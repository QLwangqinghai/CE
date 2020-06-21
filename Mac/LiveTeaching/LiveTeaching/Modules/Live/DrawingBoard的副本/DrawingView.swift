//
//  DrawingView.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
@_exported import Basic

public class BaseDrawingView: UIView {

    /*
     draw (background+content)
     shape
     foreground
     */
    
    //图形图形， 用于放临时的图形， 确认后 提交到画板上
    public let shapeView: ViewContainer

    //前景
    public let foregroundView: ViewContainer
    
    internal var drawDelegate: DrawingViewDrawDelegate? = nil

    public override init(frame: CGRect) {
        let bounds = CGRect(origin: CGPoint(), size: frame.size)
        let shapeView: ViewContainer = ViewContainer(frame: bounds)
        self.shapeView = shapeView
        
        let foregroundView: ViewContainer = ViewContainer(frame: bounds)
        foregroundView.bounds = self.bounds
        self.foregroundView = foregroundView
        
        super.init(frame: bounds)
        self.addSubview(shapeView)
        self.addSubview(foregroundView)
    }
    required init?(coder: NSCoder) {
        let shapeView: ViewContainer = ViewContainer(frame: bounds)
        self.shapeView = shapeView
        let foregroundView: ViewContainer = ViewContainer()
        self.foregroundView = foregroundView

        super.init(coder: coder)
        self.addSubview(shapeView)
        self.addSubview(foregroundView)
        shapeView.frame = self.bounds
        foregroundView.frame = self.bounds
        self.foregroundView.bounds = self.bounds
    }
    
    public override func layoutSubviews() {
        super.layoutSubviews()
        self.shapeView.frame = self.bounds
        self.foregroundView.frame = self.bounds
        self.foregroundView.bounds = self.bounds
    }

//    open func insertSubview(_ view: UIView, at index: Int)
//    open func exchangeSubview(at index1: Int, withSubviewAt index2: Int)
//    open func addSubview(_ view: UIView)
//    open func insertSubview(_ view: UIView, belowSubview siblingSubview: UIView)
//    open func insertSubview(_ view: UIView, aboveSubview siblingSubview: UIView)
//    open func bringSubviewToFront(_ view: UIView)
//    open func sendSubviewToBack(_ view: UIView)

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

    public let identifier: AnyHashable
    public let timestamp: TimeInterval
    
    public private(set) var items: [TouchPoint]

    public init(identifier: AnyHashable, point: TouchPoint) {
        self.identifier = identifier
        self.timestamp = point.timestamp
        self.bounds = (point.location, point.location)
        self.items = [point]
    }
    
    internal func append(_ point: TouchPoint) {
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
    
    public func touchesBegan(view: TiledDrawingView, touches: Set<UITouch>, with event: UIEvent?) {
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

                        if let id = delegate.drawingView(view, shouldBeginRecognizeAt: point) {
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
    
    public func touchesMoved(view: TiledDrawingView, touches: Set<UITouch>, with event: UIEvent?) {
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
    
    private func _touchesEnded(view: TiledDrawingView, touches: Set<UITouch>, with event: UIEvent?) {
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

    
    public func touchesEnded(view: TiledDrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        self._touchesEnded(view: view, touches: touches, with: event)

    }
    public func touchesCancelled(view: TiledDrawingView, touches: Set<UITouch>, with event: UIEvent?) {
//        Log.logEvent(view: view, touches: touches, event: event)
        self._touchesEnded(view: view, touches: touches, with: event)
    }
}

public protocol DrawingViewDrawDelegate: class {
    
    //返回一个identifer
    func drawingView(_ view: BaseDrawingView, shouldBeginRecognizeAt point: TouchPoint) -> AnyHashable?
    
    //开始识别
    func drawingView(_ view: BaseDrawingView, didBeginRecognize context: TouchPointCollection)
     
    //识别失败
    func drawingView(_ view: BaseDrawingView, didRecognizedFailed context: TouchPointCollection)
    
    //识别成功
    func drawingView(_ view: BaseDrawingView, didRecognizedSuccess context: TouchPointCollection)
    
    func drawingView(_ view: BaseDrawingView, draw context: TouchPointCollection)
    
    func drawingView(_ view: TiledDrawingView, finish context: TouchPointCollection)
}

//
//
//public class TiledDrawingView: BaseDrawingView {
//    /*
//     background
//     content
//     shape
//     foreground
//     */
//
//    public let content: CATiledLayer
//
//    //图形容器层
//    public let status: DrawingStatus
//    public let bitmap: DrawingBitmap
//
//    private let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()
//
//    private var isDispatchEventToRecognizer: Bool = false
//
//    internal var drawDelegate: DrawingViewDrawDelegate? = nil
//
//    public init(status: DrawingStatus) {
//        self.status = status
//        let bounds = CGRect(origin: CGPoint(), size: status.drawingSize.cgSize)
//        let content = CATiledLayer()
//        content.zPosition = -1
//        content.contentsScale = UIScreen.main.scale
//        self.content = content
////        content.delegate = self
////        content.frame = bounds
//        var size = status.drawingSize.rawValue
//        size.height = Int32(status.contentHeightLimit)
//        self.bitmap = DrawingBitmap(size: size, status: status)
//        super.init(frame: bounds)
//        self.clipsToBounds = true
//        self.layer.masksToBounds = true
//        self.layer.addSublayer(content)
//        self._prepare()
//    }
//
//    required init?(coder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//    }
//
//    private func _prepare() {
//        self.isMultipleTouchEnabled = true
//    }
//    public override func layoutSubviews() {
//        super.layoutSubviews()
//    }
//    public func didUpdateStatus(from: DrawingStatus.Status, to status: DrawingStatus.Status) {
//        var bounds = self.bounds
//        bounds.origin.y = status.offset
//        self.bounds = bounds
//        self.bitmap.didUpdateStatus(from: from, to: status)
//    }
//
//    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        if self.eventRecognizer.isEnabled {
//            self.isDispatchEventToRecognizer = true
//        } else {
//            self.isDispatchEventToRecognizer = false
//        }
//        guard self.isDispatchEventToRecognizer else {
//            super.touchesBegan(touches, with: event)
//            return
//        }
//        self.eventRecognizer.touchesBegan(view: self, touches: touches, with: event)
//    }
//    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard self.isDispatchEventToRecognizer else {
//            super.touchesMoved(touches, with: event)
//            return
//        }
//        self.eventRecognizer.touchesMoved(view: self, touches: touches, with: event)
//    }
//    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard self.isDispatchEventToRecognizer else {
//            super.touchesEnded(touches, with: event)
//            return
//        }
//        guard self.eventRecognizer.isEnabled else {
//            return
//        }
//        self.eventRecognizer.touchesEnded(view: self, touches: touches, with: event)
//    }
//    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard self.isDispatchEventToRecognizer else {
//            super.touchesCancelled(touches, with: event)
//            return
//        }
//        guard self.eventRecognizer.isEnabled else {
//            return
//        }
//        self.eventRecognizer.touchesCancelled(view: self, touches: touches, with: event)
//    }
//}
//
//


public class TiledDrawingView: BaseDrawingView {
    /*
     background
     content
     shape
     foreground
     */
    
    public let content: CATiledLayer
    
    public let status: DrawingStatus
    public let bitmap: DrawingBitmap
    
    private let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()

    private var isDispatchEventToRecognizer: Bool = false
    

    public init(status: DrawingStatus) {
        self.status = status
        let bounds = CGRect(origin: CGPoint(), size: status.drawingSize.cgSize)
        let content = CATiledLayer()
        content.zPosition = -1
        content.contentsScale = UIScreen.main.scale
        self.content = content
//        content.delegate = self
//        content.frame = bounds
        var size = status.drawingSize.rawValue
        size.height = Int32(status.contentHeightLimit)
        self.bitmap = DrawingBitmap(size: size, status: status)
        super.init(frame: bounds)
        self.clipsToBounds = true
        self.layer.masksToBounds = true
        self.layer.addSublayer(content)
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



