//
//  Path.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
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
