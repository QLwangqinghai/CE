//
//  SamplePointView.swift
//  TD
//
//  Created by vector on 2020/1/8.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

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


public class SamplePoint: CustomDebugStringConvertible {
    public enum SampleType: UInt32 {
        case normal
        case interpolation
    }
    
    public let type: SampleType
    public let point: Point64
    public let time: Int64
    public var velocity: Point64
    public var acceleration: Point64

    public init(type: SampleType, point: Point64, time: Int64, velocity: Point64 = Point64(), acceleration: Point64 = Point64()) {
        self.type = type
        self.point = point
        self.time = time
        self.velocity = velocity
        self.acceleration = acceleration
    }
    
    public init(type: SampleType, cgPoint: CGPoint, timeInterval: TimeInterval) {
        self.type = type
        self.point = Point64(cgPoint: cgPoint, scale: 10000)
        self.time = Int64(timeInterval * 1000000)
        self.velocity = Point64()
        self.acceleration = Point64()
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
    
    public init() {}
    
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

            var change = Point64()
            change.x = p1.point.x - p0.point.x
            change.y = p1.point.y - p0.point.y
            let t = p1.time - p0.time

            var v = Point64()
            v.x = change.x / t
            v.y = change.y / t

            p0.velocity = v
            p1.velocity = v
            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
        }
        
        if points.count > 2 {
            var last = points[1]
            for idx in 2 ..< points.count {
                var change = Point64()
                let current = points[idx]
                change.x = current.point.x - last.point.x
                change.y = current.point.y - last.point.y
                let t = current.time - last.time
                let xa = (change.x - last.velocity.x * t) * 2 / t / t
                last.acceleration.x = xa
                
                let ya = (change.y - last.velocity.y * t) * 2 / t / t
                last.acceleration.y = ya

                current.velocity.x = change.x * 2 / t - last.velocity.x
                current.velocity.y = change.y * 2 / t - last.velocity.y
                last = current
            }
            
            last = points[0]
            for idx in 1 ..< points.count {
                result.append(last)
                let current = points[idx]
                let offset = (current.time - last.time) / 16
                for idx in 1 ..< 16 {
                    let go = offset * Int64(idx)
                    let t = last.time + Int64(go)
                    
                    var v = Point64()
                    v.x = last.velocity.x + last.acceleration.x * go
                    v.y = last.velocity.y + last.acceleration.y * go

                    var p = Point64()
                    p.x = last.point.x + (last.velocity.x + v.x) * go / 2
                    p.y = last.point.y + (last.velocity.y + v.y) * go / 2

                    let sp = SamplePoint(type: .interpolation, point: p, time: t, velocity: v)
                    result.append(sp)
                }
                last = current
            }
            result.append(last)

            
        }
        print("[\(points)] fps:\(fps)")
        return result
    }

    
    func log() {
        let points = self.points
        var fps: Int = 60

        
        if points.count >= 2 {
            let p0 = points[0]
            let p1 = points[1]

            var v = Point64()
            v.x = p1.point.x - p0.point.x
            v.y = p1.point.y - p0.point.y
            p0.velocity = v
            p1.velocity = v
            fps = Int((self.points.count - 1) * 1000000 / Int(points.last!.time - points[0].time))
        }
        
        if points.count > 2 {
            var last = points[1]
            for idx in 2 ..< points.count {
                var change = Point64()
                let current = points[idx]
                change.x = current.point.x - last.point.x
                change.y = current.point.y - last.point.y
                let t = current.time - last.time
                let xa = (Int64(change.x) - Int64(last.velocity.x) * t) * 2 / t / t
                last.acceleration.x = xa
                
                let ya = (Int64(change.y) - Int64(last.velocity.y) * t) * 2 / t / t
                last.acceleration.y = ya

                current.velocity.x = Int64(change.x) * 2 / t - Int64(last.velocity.x)
                current.velocity.y = Int64(change.y) * 2 / t - Int64(last.velocity.y)
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
