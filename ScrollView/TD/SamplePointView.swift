//
//  SamplePointView.swift
//  TD
//
//  Created by vector on 2020/1/8.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

public class SamplePoint: CustomDebugStringConvertible {
    public let point: CGPoint
    public let time: TimeInterval
    public let velocity: CGPoint
    
    public init(point: CGPoint, time: TimeInterval, velocity: CGPoint) {
        self.point = point
        self.time = time
        self.velocity = velocity
    }
    
    
    public var debugDescription: String {
        return "(\(point), at:\(time), v:\(velocity))"
    }
}


public class SamplePointBuffer {
    public private(set) var points: [(CGPoint, time: TimeInterval)] = []
    
    public func append(point: CGPoint, time: TimeInterval) {
        self.points.append((point, time))
    }
    func finish() {
        
    }

    
    public init() {}
    
    func log() {
        let points = self.points
        var items: [SamplePoint] = []
        items.reserveCapacity(points.count)
        if points.count == 0 {
        } else if points.count == 1 {
            let sp = SamplePoint(point: points[0].0, time: points[0].1, velocity: CGPoint())
            items.append(sp)
        } else {
            let sp = SamplePoint(point: points[0].0, time: points[0].1, velocity: CGPoint())
            items.append(sp)

            var velocitys: [CGPoint] = []
            velocitys.reserveCapacity(points.count + 3)
            var last = points[0]
            for idx in 1 ..< points.count {
                var v = CGPoint()
                let current = points[idx]
                v.x = current.0.x - last.0.x
                v.y = current.0.y - last.0.y
                v.x = v.x / CGFloat((current.1 - last.1))
                v.y = v.y / CGFloat((current.1 - last.1))
                velocitys.append(v)
                last = current
            }
            
            
            for idx in 0 ..< points.count {
                let p0 = velocitys[idx]
                let p1 = velocitys[idx + 1]
                let p2 = velocitys[idx + 2]
                let p3 = velocitys[idx + 3]
                var v = CGPoint()
                v.x = p0.x / 8 + p1.x * 0.375 + p2.x * 0.375 + p3.x / 8
                v.y = p0.y / 8 + p1.y * 0.375 + p2.y * 0.375 + p3.y / 8
                let sp = SamplePoint(point: points[idx].0, time: points[idx].1, velocity: v)
                items.append(sp)
            }
        }
        print(items)
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
