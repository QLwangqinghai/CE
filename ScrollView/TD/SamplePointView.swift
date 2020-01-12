//
//  SamplePointView.swift
//  TD
//
//  Created by vector on 2020/1/8.
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


public struct Point64 {
    public var x: Int64
    public var y: Int64
    public init(x: Int64, y: Int64) {
        self.x = x
        self.y = y
    }
    public init() {
        self.x = 0
        self.y = 0
    }
    
    init(cgPoint: CGPoint, scale: CGFloat) {
        self.init(x: Int64(cgPoint.x * scale), y: Int64(cgPoint.y * scale))
    }
}

public class Vector {
    public var value: Int64
    public let time: Int64
    public var velocity: Int64
    public var acceleration: Int64
    public var velocityOffset: Int64 = 0
    public var aa: Int64 = 0
    init(value: Int64, time: Int64) {
        self.value = value
        self.time = time
        self.velocity = 0
        self.acceleration = 0
    }
    convenience init(value: CGFloat, scale: CGFloat, timeInterval: TimeInterval) {
        self.init(value: Int64(value * scale), time: Int64(timeInterval * 1000))
    }
}

public class SamplePoint: CustomDebugStringConvertible {
    public enum SampleType: UInt32 {
        case normal
        case interpolation
    }
    
    public let type: SampleType
    
    public var x: Vector
    public var y: Vector
    public let time: Int64
    
    public var point: CGPoint {
        return CGPoint(x: Double(self.x.value) / 1000000, y: Double(self.y.value) / 1000000)
    }
    
    public init(type: SampleType, point: CGPoint, timeInterval: TimeInterval) {
        self.type = type
        self.x = Vector(value: point.x, scale: 1000000, timeInterval: timeInterval)
        self.y = Vector(value: point.y, scale: 1000000, timeInterval: timeInterval)
        self.time = Int64(timeInterval * 1000)
    }
    
    public init(type: SampleType, x: Vector, y: Vector, time: Int64) {
        self.type = type
        self.x = x
        self.y = y
        self.time = time
    }
    
    
    public var debugDescription: String {
        return "(\(type): \(x) \(y), at:\(time))"
    }
}


public class SamplePointBuffer {
    public private(set) var points: [SamplePoint] = []
    
    public func append(point: CGPoint, time: TimeInterval) {
        self.points.append(SamplePoint(type: .normal, point: point, timeInterval: time))
    }
    
    public init() {}
    
    
    public func velocity(of vector: Vector, next: (Vector, Int64)) -> Int64 {
        return (next.0.value - vector.value) / next.1
    }
    public func velocity(of vector: Vector, prev: (Vector, Int64)) -> Int64 {
        return (vector.value - prev.0.value) / prev.1
    }
    public func velocity(of vector: Vector, prev: (Vector, Int64), next: (Vector, Int64)) -> Int64 {
        return ((vector.value - prev.0.value) / prev.1 + (next.0.value - vector.value) / next.1) / 2
    }
    
    public func velocity(p0: Vector, p1: Vector, t: Int64) -> Int64 {
        let change = p1.value - p0.value
        return change / t
    }
    public func acceleration(p0: Vector, p1: Vector, t: Int64) -> Int64 {
        let change = p1.velocity - p0.velocity
        return change / t
    }
    public func velocityOffset(p0: Vector, p1: Vector, t: Int64) -> Int64 {
        let change = p1.value - p0.value
        let total = change - (p0.velocity + p1.velocity) * t / 2
        return (total - p0.aa * t * t / 2) / t
    }
    
    public func finish() -> [SamplePoint] {
        guard self.points.count > 0 else {
            return []
        }
        
        var result: [SamplePoint] = []
        let points = self.points
        var fps: Int = 60

        if points.count >= 2 {
            let p0 = points[0]
            let p1 = points[1]
            let dt = p1.time - p0.time
            let velocityX = self.velocity(p0: p0.x, p1: p1.x, t: dt)
            let velocityY = self.velocity(p0: p0.y, p1: p1.y, t: dt)
            p0.x.velocity = velocityX
            p0.y.velocity = velocityY
            p1.x.velocity = velocityX
            p1.y.velocity = velocityY
            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
        }
        
        if points.count > 2 {
            var last = points[0]
            for idx in 1 ..< points.count {
                let current = points[idx]

                if idx != points.count - 1 {
                    let next = points[idx + 1]
                    let dt = next.time - last.time

                    current.x.velocity = self.velocity(of: current.x, prev: (last.x, current.time - last.time), next: (next.x, dt))
                    current.y.velocity = self.velocity(of: current.y, prev: (last.y, current.time - last.time), next: (next.y, dt))
                    if idx == 1 {
                        last.x.velocity = self.velocity(of: last.x, next: (next.x, dt))
                        last.y.velocity = self.velocity(of: last.y, next: (next.y, dt))
                    }
                } else {
                    current.x.velocity = self.velocity(of: current.x, prev: (last.x, current.time - last.time))
                    current.y.velocity = self.velocity(of: current.y, prev: (last.y, current.time - last.time))
                }

                
                let dt = current.time - last.time
                last.x.acceleration = self.acceleration(p0: last.x, p1: current.x, t: dt)
                last.y.acceleration = self.acceleration(p0: last.y, p1: current.y, t: dt)

                last = current
            }


            //加速度变化
            last = points[0]
            for idx in 1 ..< points.count {
                let current = points[idx]
                let dt = current.time - last.time
                last.x.aa = (current.x.acceleration - last.x.acceleration) / dt * 1
                last.y.aa = (current.y.acceleration - last.y.acceleration) / dt * 1

                last.x.velocityOffset = self.velocityOffset(p0: last.x, p1: current.x, t: dt)
                last.y.velocityOffset = self.velocityOffset(p0: last.y, p1: current.y, t: dt)
                last = current
            }
            
            last = points[0]
            for idx in 1 ..< points.count - 1 {
                result.append(last)
                let current = points[idx]
                let offset = (current.time - last.time) / 16
                for idx in 1 ..< 16 {
                    let go = offset * Int64(idx)
                    let t = last.time + Int64(go)
                    
                    let x = Vector(value: last.x.value + (last.x.velocity + current.x.velocity) * go / 2 + last.x.velocityOffset * go + last.x.aa * go * go / 2, time: t)
                    let y = Vector(value: last.y.value + (last.y.velocity + current.y.velocity) * go / 2 + last.y.velocityOffset * go + last.y.aa * go * go / 2, time: t)
                    let sp = SamplePoint(type: .interpolation, x: x, y: y, time: t)
                    result.append(sp)
                }
                last = current
            }
            result.append(last)
            
        }
        print("[\(points)] fps:\(fps)")
        return result
    }

    
    
//    public func finish() -> [SamplePoint] {
//        guard self.points.count > 0 else {
//            return []
//        }
//
//        var result: [SamplePoint] = []
//        let points = self.points
//        var fps: Int = 60
//
//        if points.count >= 2 {
//            let p0 = points[0]
//            let p1 = points[1]
//            let dt = p1.time - p0.time
//            let velocityX = self.velocity(p0: p0.x, p1: p1.x, t: dt)
//            let velocityY = self.velocity(p0: p0.y, p1: p1.y, t: dt)
//            p0.x.velocity = velocityX
//            p0.y.velocity = velocityY
//            p1.x.velocity = velocityX
//            p1.y.velocity = velocityY
//            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
//        }
//
//        if points.count > 2 {
//            var last = points[0]
//            for idx in 1 ..< points.count {
//                let current = points[idx]
//
//                if idx != points.count - 1 {
//                    let next = points[idx + 1]
//                    let dt = next.time - last.time
//
//                    current.x.velocity = self.velocity(p0: last.x, p1: next.x, t: dt)
//                    current.y.velocity = self.velocity(p0: last.y, p1: next.y, t: dt)
//                    if idx == 1 {
//                        last.x.velocity = current.x.velocity / 2
//                        last.y.velocity = current.x.velocity / 2
//                    }
//                } else {
//                    current.x.velocity = last.x.velocity / 2
//                    current.y.velocity = last.x.velocity / 2
//                }
//
//
//                let dt = current.time - last.time
//                last.x.acceleration = self.acceleration(p0: last.x, p1: current.x, t: dt)
//                last.y.acceleration = self.acceleration(p0: last.y, p1: current.y, t: dt)
//
//                last = current
//            }
//
//
//            //加速度变化
//            last = points[0]
//            for idx in 1 ..< points.count {
//                let current = points[idx]
//                let dt = current.time - last.time
//                last.x.aa = (current.x.acceleration - last.x.acceleration) / dt
//                last.y.aa = (current.y.acceleration - last.y.acceleration) / dt
//
//                last.x.velocityOffset = self.velocityOffset(p0: last.x, p1: current.x, t: dt)
//                last.y.velocityOffset = self.velocityOffset(p0: last.y, p1: current.y, t: dt)
//                last = current
//            }
//
//            last = points[0]
//            for idx in 1 ..< points.count - 1 {
//                result.append(last)
//                let current = points[idx]
//                let offset = (current.time - last.time) / 16
//                for idx in 1 ..< 16 {
//                    let go = offset * Int64(idx)
//                    let t = last.time + Int64(go)
//
//                    let x = Vector(value: last.x.value + (last.x.velocity + current.x.velocity) * go / 2 + last.x.velocityOffset * go + last.x.aa * go * go / 2, time: t)
//                    let y = Vector(value: last.y.value + (last.y.velocity + current.y.velocity) * go / 2 + last.y.velocityOffset * go + last.y.aa * go * go / 2, time: t)
//                    let sp = SamplePoint(type: .interpolation, x: x, y: y, time: t)
//                    result.append(sp)
//                }
//                last = current
//            }
//            result.append(last)
//
//        }
//        print("[\(points)] fps:\(fps)")
//        return result
//    }
    
    
//    public func finish() -> [SamplePoint] {
//        guard self.points.count > 0 else {
//            return []
//        }
//
//        var result: [SamplePoint] = []
//        let points = self.points
//        var fps: Int = 60
//
//
//        if points.count >= 2 {
//            let p0 = points[0]
//            let p1 = points[1]
//
//            var change = Point64()
//            change.x = p1.point.x - p0.point.x
//            change.y = p1.point.y - p0.point.y
//            let t = p1.time - p0.time
//
//            var v = Point64()
//            v.x = change.x / t
//            v.y = change.y / t
//
//            p0.velocity = v
//            p1.velocity = v
//            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
//        }
//
//        if points.count > 2 {
//            var last = points[1]
//            for idx in 2 ..< points.count {
//                var change = Point64()
//                let current = points[idx]
//                change.x = current.point.x - last.point.x
//                change.y = current.point.y - last.point.y
//                let t = current.time - last.time
//                let xa = (change.x - last.velocity.x * t) * 2 / t / t
//                last.acceleration.x = xa
//
//                let ya = (change.y - last.velocity.y * t) * 2 / t / t
//                last.acceleration.y = ya
//
//                current.velocity.x = change.x * 2 / t - last.velocity.x
//                current.velocity.y = change.y * 2 / t - last.velocity.y
//                last = current
//            }
//
//            last = points[0]
//            for idx in 1 ..< points.count {
//                result.append(last)
//                let current = points[idx]
//                let offset = (current.time - last.time) / 16
//                for idx in 1 ..< 16 {
//                    let go = offset * Int64(idx)
//                    let t = last.time + Int64(go)
//
//                    var v = Point64()
//                    v.x = last.velocity.x + last.acceleration.x * go
//                    v.y = last.velocity.y + last.acceleration.y * go
//
//                    var p = Point64()
//                    p.x = last.point.x + (last.velocity.x + v.x) * go / 2
//                    p.y = last.point.y + (last.velocity.y + v.y) * go / 2
//
//                    let sp = SamplePoint(type: .interpolation, point: p, time: t, velocity: v)
//                    result.append(sp)
//                }
//                last = current
//            }
//            result.append(last)
//
//
//        }
//        print("[\(points)] fps:\(fps)")
//        return result
//    }

    
//    func log() {
//        let points = self.points
//        var fps: Int = 60
//        if points.count >= 2 {
//            let p0 = points[0]
//            let p1 = points[1]
//            var v = Point64()
//            v.x = p1.point.x - p0.point.x
//            v.y = p1.point.y - p0.point.y
//            p0.velocity = v
//            p1.velocity = v
//            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
//        }
//        if points.count > 2 {
//            var last = points[1]
//            for idx in 2 ..< points.count {
//                var change = Point64()
//                let current = points[idx]
//                change.x = current.point.x - last.point.x
//                change.y = current.point.y - last.point.y
//                let t = current.time - last.time
//                let xa = (Int64(change.x) - Int64(last.velocity.x) * t) * 2 / t / t
//                last.acceleration.x = xa
//
//                let ya = (Int64(change.y) - Int64(last.velocity.y) * t) * 2 / t / t
//                last.acceleration.y = ya
//
//                current.velocity.x = Int64(change.x) * 2 / t - Int64(last.velocity.x)
//                current.velocity.y = Int64(change.y) * 2 / t - Int64(last.velocity.y)
//                last = current
//            }
//        }
//        print("[\(points)] fps:\(fps)")
//    }
}

public struct TouchPoint {
    public var location: CGPoint
//    public var velocity: CGPoint
    public var time: TimeInterval
    
    public init(location: CGPoint = CGPoint(), time: TimeInterval = 0) {
        self.location = location
//        self.velocity = velocity
        self.time = time
    }
}

public protocol TouchEventHandleable: class {
    func begin(_ point: TouchPoint)
    func change(_ point: TouchPoint)
    func finish(_ point: TouchPoint)
}

public class DrawingEventHandler: TouchEventHandleable {
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



public final class GestureEventHandler: NSObject {
    public let panGestureRecognizer: UIPanGestureRecognizer = UIPanGestureRecognizer()
    
    override init() {
        super.init()
        self.panGestureRecognizer.addTarget(self, action: #selector(GestureEventHandler.handlePan))
    }
    
    public var handler: TouchEventHandleable?

    @objc private func handlePan(recognizer: UIPanGestureRecognizer) {
        guard recognizer == self.panGestureRecognizer else {
            return
        }
        let location = recognizer.location(in: recognizer.view)
        let point = TouchPoint(location: location, velocity: recognizer.velocity(in: recognizer.view), time: CACurrentMediaTime())

        guard let handler = self.handler else {
            return
        }
        
        if recognizer.state == .began {
            handler.begin(point)
        } else if recognizer.state == .changed {
            handler.change(point)
        } else {
            if recognizer.state == .cancelled || recognizer.state == .ended {
                handler.finish(point)
            }
        }
    }
}

public class SamplePointView: UIView {

    public var buffers: [SamplePointBuffer] = []
    
    
    public var buffer: SamplePointBuffer?
    var aa: UIPanGestureRecognizer?
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        let buffer = SamplePointBuffer()
        buffer.append(point: touch.location(in: self), time: touch.timestamp)
        self.buffer = buffer
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        if let buffer = self.buffer {
            buffer.append(point: touch.location(in: self), time: touch.timestamp)
        }
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        if let buffer = self.buffer {
            buffer.append(point: touch.location(in: self), time: touch.timestamp)
            self.append(buffer)
        }
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        if let buffer = self.buffer {
            buffer.append(point: touch.location(in: self), time: touch.timestamp)
            self.append(buffer)
        }
    }
    
    func append(_ buffer: SamplePointBuffer) {
        self.buffers.append(buffer)
                
    }
}
