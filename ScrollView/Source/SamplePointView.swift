//
//  SamplePointView.swift
//  TD
//
//  Created by vector on 2020/1/8.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

public extension Point {
    
    init(cgPoint: CGPoint, scale: CGFloat) {
        self.init(x: Int32(cgPoint.x * scale), y: Int32(cgPoint.y * scale))
    }
}


public class SamplePoint: CustomDebugStringConvertible {
    public enum SampleType: UInt32 {
        case normal
        case interpolation
    }
    
    public let type: SampleType
    public let point: Point
    public let time: Int64
    public var velocity: Point
    public var acceleration: Point

    public init(type: SampleType, point: Point, time: Int64, velocity: Point = Point(), acceleration: Point = Point()) {
        self.type = type
        self.point = point
        self.time = time
        self.velocity = velocity
        self.acceleration = acceleration
    }
    
    public init(type: SampleType, cgPoint: CGPoint, timeInterval: TimeInterval) {
        self.type = type
        self.point = Point(cgPoint: cgPoint, scale: 10000)
        self.time = Int64(timeInterval * 1000000)
        self.velocity = Point()
        self.acceleration = Point()
    }
    
    
    public var debugDescription: String {
        return "(\(type): \(point), at:\(time), v:\(velocity), a:\(acceleration))"
    }
}


public class SamplePointBuffer {
    public private(set) var points: [SamplePoint] = []
    
    public func append(point: CGPoint, time: TimeInterval) {
        self.points.append(SamplePoint(type: .normal, cgPoint: point, timeInterval: time))
    }
    func finish() {
        
    }

    
    public init() {}
    
    func log() {
        let points = self.points
        var fps: Int = 60

        
        if points.count >= 2 {
            let p0 = points[0]
            let p1 = points[1]

            var v = Point()
            v.x = p1.point.x - p0.point.x
            v.y = p1.point.y - p0.point.y
            p0.velocity = v
            p1.velocity = v
            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
        }
        
        if points.count > 2 {
            var last = points[1]
            for idx in 2 ..< points.count {
                var change = Point()
                let current = points[idx]
                change.x = current.point.x - last.point.x
                change.y = current.point.y - last.point.y
                let t = current.time - last.time
                let xa = Int32((Int64(change.x) - Int64(last.velocity.x) * t) * 2 / t / t)
                last.acceleration.x = xa
                
                let ya = Int32((Int64(change.y) - Int64(last.velocity.y) * t) * 2 / t / t)
                last.acceleration.y = ya

                current.velocity.x = Int32(Int64(change.x) * 2 / t - Int64(last.velocity.x))
                current.velocity.y = Int32(Int64(change.y) * 2 / t - Int64(last.velocity.y))
                last = current
            }
        }
        print("[\(points)] fps:\(fps)")
        
        
    }
}



public class SamplePointView: UIView {

    public var buffers: [SamplePointBuffer] = []
    
    
    public var buffer: SamplePointBuffer?
    
    
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
        
        buffer.log()
        
    }
}
