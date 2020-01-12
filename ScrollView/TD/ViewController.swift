//
//  ViewController.swift
//  TD
//
//  Created by vector on 2020/1/5.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit
import UIKit.UIGestureRecognizerSubclass

public enum ColorSpace: UInt32 {
    //android 只有argb模式
    
    case little32Argb = 1
    case little16Xrgb = 2
    public static let deviceRgb: CGColorSpace = CGColorSpaceCreateDeviceRGB()

    public var bytesPerPixel: UInt32 {
        switch self {
        case .little32Argb:
            return 4
        case .little16Xrgb:
            return 2
        }
    }
    public var bitsPerComponent: Int {
        switch self {
        case .little32Argb:
            return 8
        case .little16Xrgb:
            return 5
        }
    }
    public var bitsPerPixel: Int {
        switch self {
        case .little32Argb:
            return 32
        case .little16Xrgb:
            return 16
        }
    }
    
    
    public var bitmapInfo: UInt32 {
        switch self {
        case .little32Argb:
            return CGBitmapInfo.byteOrder32Little.rawValue | CGImageAlphaInfo.premultipliedFirst.rawValue
        case .little16Xrgb:
            return CGBitmapInfo.byteOrder16Little.rawValue | CGImageAlphaInfo.noneSkipFirst.rawValue
        }
    }
    
    
}

open class DrawingPanGestureRecognizer: UIPanGestureRecognizer {
    
    public private(set) var timestamp: TimeInterval = 0

    public private(set) var sequence: Int = 0

    
    public override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent) {
        print("begin  count:\(touches.count) ")
        self.sequence += 1
        self.timestamp = event.timestamp
        super.touchesBegan(touches, with: event)
    }
    public override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent) {
        print("touchesMoved count:\(touches.count) ")

        self.timestamp = event.timestamp
        super.touchesMoved(touches, with: event)
    }
    public override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent) {
        print("touchesEnded   \(touches.count)")

        self.timestamp = event.timestamp
        super.touchesEnded(touches, with: event)
    }
    public override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent) {
        print("touchesCancelled   \(touches.count)")

        self.timestamp = event.timestamp
        super.touchesCancelled(touches, with: event)
    }
    public override func touchesEstimatedPropertiesUpdated(_ touches: Set<UITouch>) {
        print("touchesEstimatedPropertiesUpdated   \(touches.count)")
//        self.timestamp = event.timestamp
        super.touchesEstimatedPropertiesUpdated(touches)
    }

}



//open class DrawingView: UIView {
//    public let topLayer: CALayer
//    public let bottomLayer: CALayer
//
//    public let topContext: CGContext
//    public let bottomContext: CGContext
//
//    public init(width: UInt32, height: UInt32) {
//        assert(height % 2 == 0)
//        let scale = UIScreen.main.scale
//        let frame: CGRect = CGRect(x: 0, y: 0, width: CGFloat(width) / scale, height: CGFloat(height) / scale)
//        let topLayer: CALayer = CALayer()
//        let bottomLayer: CALayer = CALayer()
//        self.topLayer = topLayer;
//        self.bottomLayer = bottomLayer
//
//        let top = UnsafeMutableRawPointer.allocate(byteCount: Int(width * height * 2), alignment: 128)
//        let bottom = UnsafeMutableRawPointer.allocate(byteCount: Int(width * height * 2), alignment: 128)
//
//        let topContext = CGContext(data: top, width: Int(width), height: Int(height/2), bitsPerComponent: 8, bytesPerRow: Int(width * 4), space: ColorSpace.deviceRgb, bitmapInfo: ColorSpace.little32Argb.bitmapInfo, releaseCallback: { (context, ptr) in
//        }, releaseInfo: nil)!
//        self.topContext = topContext
//        let bottomContext = CGContext(data: bottom, width: Int(width), height: Int(height/2), bitsPerComponent: 8, bytesPerRow: Int(width * 4), space: ColorSpace.deviceRgb, bitmapInfo: ColorSpace.little32Argb.bitmapInfo, releaseCallback: { (context, ptr) in
//        }, releaseInfo: nil)!
//        self.bottomContext = bottomContext
//
//
//        topContext.setLineWidth(3)
//        topContext.setStrokeColor(red: 1, green: 0, blue: 0, alpha: 1)
//        topContext.translateBy(x: 0, y: CGFloat(height/2))
//        topContext.scaleBy(x: 1.0, y: -1.0)
//
//        bottomContext.setLineWidth(3)
//        bottomContext.setStrokeColor(red: 1, green: 0, blue: 0, alpha: 1)
////        bottomContext.setStrokeColor(red: 0, green: 0, blue: 1, alpha: 1)
//        bottomContext.translateBy(x: 0, y: CGFloat(height/2))
//        bottomContext.scaleBy(x: 1.0, y: -1.0)
//        bottomContext.translateBy(x: 0, y: (-1.0) * CGFloat(height/2))
//
//        super.init(frame: frame)
//        var bounds = self.bounds
//        bounds.size.height = bounds.height / 2
//        topLayer.frame = bounds
//        bounds.origin.y += bounds.height
//        bottomLayer.frame = bounds
//
//        self.layer.addSublayer(topLayer)
//        self.layer.addSublayer(bottomLayer)
//    }
//
//    required public init?(coder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//    }
//
//    var points: [CGPoint] = []
//
//
//
//    public var buffers: [SamplePointBuffer] = []
//
//
//    public var buffer: SamplePointBuffer?
//
//
//    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let touch = touches.first else {
//            return
//        }
//        let buffer = SamplePointBuffer()
//        buffer.append(point: touch.location(in: self), time: touch.timestamp)
//        self.buffer = buffer
//    }
//    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        print("---[moved]")
//        guard let touch = touches.first else {
//            return
//        }
//        if let buffer = self.buffer {
//            buffer.append(point: touch.location(in: self), time: touch.timestamp)
//        }
//    }
//    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let touch = touches.first else {
//            return
//        }
//        if let buffer = self.buffer {
//            buffer.append(point: touch.location(in: self), time: touch.timestamp)
//            self.append(buffer)
//        }
//    }
//    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        guard let touch = touches.first else {
//            return
//        }
//        if let buffer = self.buffer {
//            buffer.append(point: touch.location(in: self), time: touch.timestamp)
//            self.append(buffer)
//        }
//    }
//
//    func append(_ buffer: SamplePointBuffer) {
//        self.buffers.append(buffer)
//
//        let ps = buffer.finish()
//
//        let points: [CGPoint] = self.points
//        self.points.removeAll()
//
//        let path = UIBezierPath()
//        path.lineWidth = 2
//
//        let scale = UIScreen.main.scale
//
//        for (index, sp) in ps.enumerated() {
//            let p = CGPoint(x: sp.point.x * scale, y: sp.point.y * scale)
//
//            let rect = CGRect(origin: p, size: CGSize(width: 1 * scale, height: 2 * scale))
//            if sp.type == .normal {
//                self.topContext.setFillColor(red: 1, green: 0, blue: 0, alpha: 1)
//                self.bottomContext.setFillColor(red: 1, green: 0, blue: 0, alpha: 1)
//            } else {
//                self.topContext.setFillColor(red: 0, green: 1, blue: 0, alpha: 1)
//                self.bottomContext.setFillColor(red: 0, green: 1, blue: 0, alpha: 1)
//            }
//
//            self.topContext.fill(rect)
//            self.bottomContext.fill(rect)
//
////            if index == 0 {
////                self.topContext.move(to: p)
////                self.bottomContext.move(to: p)
////            } else {
////                self.topContext.addLine(to: p)
////                self.bottomContext.addLine(to: p)
////            }
//        }
//        self.topContext.strokePath()
//        self.bottomContext.strokePath()
//
//        self.bottomContext.setFillColor(red: 0, green: 0, blue: 0, alpha: 1)
//        self.bottomContext.fill(CGRect(x: 30, y: 30, width: 30, height: 30))
//
////        self.topContext.addPath(path.cgPath)
////        self.bottomContext.addPath(path.cgPath)
//
//        let image = self.topContext.makeImage()
//        self.topLayer.contents = image
//        self.bottomLayer.contents = self.bottomContext.makeImage()
//
//
//
//    }
//
//
//
//
//    func finish() {
//        let points: [CGPoint] = self.points
//        self.points.removeAll()
//
//        let path = UIBezierPath()
//        path.lineWidth = 2
//
//        let scale = UIScreen.main.scale
//        for (index, point) in points.enumerated() {
//            let p = CGPoint(x: point.x * scale, y: point.y * scale)
//            if index == 0 {
//                self.topContext.move(to: p)
//                self.bottomContext.move(to: p)
//            } else {
//                self.topContext.addLine(to: p)
//                self.bottomContext.addLine(to: p)
//            }
//        }
//        self.topContext.strokePath()
//        self.bottomContext.strokePath()
//
//        self.bottomContext.fill(CGRect(x: 30, y: 30, width: 30, height: 30))
//        self.bottomContext.setFillColor(red: 0, green: 0, blue: 0, alpha: 1)
//
////        self.topContext.addPath(path.cgPath)
////        self.bottomContext.addPath(path.cgPath)
//
//        let image = self.topContext.makeImage()
//        self.topLayer.contents = image
//        self.bottomLayer.contents = self.bottomContext.makeImage()
//
//    }
//
//    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
////        super.touchesCancelled(touches, with: event)
////        self.log(item: "\(touches) \(String(describing: event))")
//    }
//
//    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
////        print("\(file) :\(line): \(function) \(item())")
//        #if os(Linux)
//
//        #elseif os(macOS)
//
//        #endif
//
//
//
//        Progress.current()
//        print("\(function) \(item())")
//    }
//}

//public struct TouchPoint {
//    public var location: CGPoint
//    public var velocity: CGPoint
//    public var time: TimeInterval
//
//    public init(location: CGPoint = CGPoint(), velocity: CGPoint = CGPoint(), time: TimeInterval = 0) {
//        self.location = location
//        self.velocity = velocity
//        self.time = time
//    }
//}



public struct DrawingAttribute {
    public var lineWidth: CGFloat
    public var lineCapStyle: CGLineCap
    public var lineJoinStyle: CGLineJoin
    public var miterLimit: CGFloat // Used when lineJoinStyle is kCGLineJoinMiter

    
    /*
       switch (blendMode) {
             case kCGBlendModeNormal: {
                 strMsg = @"kCGBlendModeNormal: 正常；也是默认的模式。前景图会覆盖背景图";
                 break;
             }
             case kCGBlendModeMultiply: {
                 strMsg = @"kCGBlendModeMultiply: 正片叠底；混合了前景和背景的颜色，最终颜色比原先的都暗";
                 break;
             }
             case kCGBlendModeScreen: {
                 strMsg = @"kCGBlendModeScreen: 滤色；把前景和背景图的颜色先反过来，然后混合";
                 break;
             }
             case kCGBlendModeOverlay: {
                 strMsg = @"kCGBlendModeOverlay: 覆盖；能保留灰度信息，结合kCGBlendModeSaturation能保留透明度信息，在imageWithBlendMode方法中两次执行drawInRect方法实现我们基本需求";
                 break;
             }
             case kCGBlendModeDarken: {
                 strMsg = @"kCGBlendModeDarken: 变暗";   //将线条色变为黑色，背景色设置为目的色
                 break;
             }
             case kCGBlendModeLighten: {
                 strMsg = @"kCGBlendModeLighten: 变亮";
                 break;
             }
             case kCGBlendModeColorDodge: {
                 strMsg = @"kCGBlendModeColorDodge: 颜色变淡";
                 break;
             }
             case kCGBlendModeColorBurn: {
                 strMsg = @"kCGBlendModeColorBurn: 颜色加深";  //线条颜色（原本）加深，背景色设置为目的色
                 break;
             }
             case kCGBlendModeSoftLight: {
                 strMsg = @"kCGBlendModeSoftLight: 柔光";
                 break;
             }
             case kCGBlendModeHardLight: {
                 strMsg = @"kCGBlendModeHardLight: 强光";  //全为目的色
                 break;
             }
             case kCGBlendModeDifference: {
                 strMsg = @"kCGBlendModeDifference: 插值";
                 break;
             }
             case kCGBlendModeExclusion: {
                 strMsg = @"kCGBlendModeExclusion: 排除";
                 break;
             }
             case kCGBlendModeHue: {
                 strMsg = @"kCGBlendModeHue: 色调";
                 break;
             }
             case kCGBlendModeSaturation: {
                 strMsg = @"kCGBlendModeSaturation: 饱和度";
                 break;
             }
             case kCGBlendModeColor: {
                 strMsg = @"kCGBlendModeColor: 颜色";   //感觉将图片线条色设置为白色，背景色设置为目的色，之后再再图片上加一个有透明度的目的色
                 break;
             }
             case kCGBlendModeLuminosity: {
                 strMsg = @"kCGBlendModeLuminosity: 亮度";
                 break;
             }
                 //Apple额外定义的枚举
                 //R: premultiplied result, 表示混合结果
                 //S: Source, 表示源颜色(Sa对应透明度值: 0.0-1.0)
                 //D: destination colors with alpha, 表示带透明度的目标颜色(Da对应透明度值: 0.0-1.0)
             case kCGBlendModeClear: {
                 strMsg = @"kCGBlendModeClear: R = 0"; //1.清空（如果图标背景色为白色则为全白）
                 break;
             }
             case kCGBlendModeCopy: {
                 strMsg = @"kCGBlendModeCopy: R = S";  //2全色覆盖整个图片
                 break;
             }
             case kCGBlendModeSourceIn: {
                 strMsg = @"kCGBlendModeSourceIn: R = S*Da";  //3.线条变色
                 break;
             }
             case kCGBlendModeSourceOut: {
                 strMsg = @"kCGBlendModeSourceOut: R = S*(1 - Da)";  //4.背景变为目的色,线条自动变为白色（比如图标线条原为蓝色，会自动变为白色）
                 break;
             }
             case kCGBlendModeSourceAtop: {
                 strMsg = @"kCGBlendModeSourceAtop: R = S*Da + D*(1 - Sa)";   //5.线条变色，目前感觉和SourceIn效果一致
                 break;
             }
             case kCGBlendModeDestinationOver: {
                 strMsg = @"kCGBlendModeDestinationOver: R = S*(1 - Da) + D";  //6.背景色变为目的色，线条色不变
                 break;
             }
             case kCGBlendModeDestinationIn: {
                 strMsg = @"kCGBlendModeDestinationIn: R = D*Sa；能保留透明度信息";  //7.只看到线条色（本色），无其他颜色
                 break;
             }
             case kCGBlendModeDestinationOut: {
                 strMsg = @"kCGBlendModeDestinationOut: R = D*(1 - Sa)";     //8.空白什么都没哟
                 break;
             }
             case kCGBlendModeDestinationAtop: {
                 strMsg = @"kCGBlendModeDestinationAtop: R = S*(1 - Da) + D*Sa";  //9.会把整个矩形的背景填充目的色（如图9系列）原色保留
                 break;
             }
             case kCGBlendModeXOR: {
                 strMsg = @"kCGBlendModeXOR: R = S*(1 - Da) + D*(1 - Sa)";  //10.线条变白，背景色变为目的色
                 break;
             }
             case kCGBlendModePlusDarker: {
                 strMsg = @"kCGBlendModePlusDarker: R = MAX(0, (1 - D) + (1 - S))";  //11.线条变为黑色， 背景色变为目的色
                 break;
             }
             case kCGBlendModePlusLighter: {
                 strMsg = @"kCGBlendModePlusLighter: R = MIN(1, S + D)（最后一种混合模式）";  //12.线条变为白色（混合色：如color为红色，就是偏粉色的白，有一定透明度的感觉）
                 break;
             }
             default: {
                 break;
             }
     */
    public var blendMode: CGBlendMode


}


open class DrawingItem: NSObject {
    
    public let id: UInt32
    private var previousPoint: CGPoint
    public private(set) var paths: [UIBezierPath] = []
    public let attribute: DrawingAttribute
    
    public var pendingPath: UIBezierPath?

    var points: [TouchPoint]
    private var pendingBeginPoint: CGPoint
    var pendingPoints: [TouchPoint]
    
    
    public func makeBezierPath() -> UIBezierPath {
        return UIBezierPath()
    }
    
    public init(id: UInt32, point: TouchPoint, attribute: DrawingAttribute) {
        self.id = id
        self.attribute = attribute
        self.points = [point]
        self.pendingPoints = [point]
        self.changedCount = 0
        self.repeatCount = 0
        self.previousPoint = point.location
        self.pendingBeginPoint = point.location
        super.init()
        
        let path = self.makeBezierPath()
        var p = point.location
        p.x -= attribute.lineWidth / 2

        path.move(to: p)
        p.x += attribute.lineWidth
        path.addLine(to: p)
        self.pendingPath = path
        
        self.display(path: path)
    }
    
    private func midpoint(_ p0: TouchPoint, _ p1: TouchPoint) -> CGPoint {
        return CGPoint(x: (p0.location.x + p1.location.x) / 2.0, y: (p0.location.y + p1.location.y) / 2.0)
    }
    
    var changedCount: Int = 0
    var repeatCount: Int = 0

//    private func moveIfNeeded(_ point: CGPoint, isLast: Bool) {
//        if point == self.previousPoint {
//            self.repeatCount += 1
//            if self.changedCount > 0 && self.repeatCount < 2 {
//                self.move(point, isLast: isLast)
//            }
//        } else {
//            self.move(point, isLast: isLast)
//            self.previousPoint = point
//            self.repeatCount = 0
//            self.changedCount += 1
//        }
//    }

    private func moveIfNeeded(_ point: TouchPoint, isLast: Bool) {

        self.move(point, isLast: isLast)
        self.previousPoint = point
        self.repeatCount = 0
        self.changedCount += 1
        
    }
    private func midpoint(_ p0: CGPoint, _ p1: CGPoint) -> CGPoint {
        return CGPoint(x: (p0.x + p1.x) / 2.0, y: (p0.y + p1.y) / 2.0)
    }

    var pathCount: Int = 0

    public func move(_ point: TouchPoint, isLast: Bool = false) {
        self.points.append(point)
        if isLast {
            self.path.addQuadCurve(to: point, controlPoint: self.previousPoint)
            self.display(path: path)
            self.path = UIBezierPath()
        } else {
            let midPoint = self.midpoint(self.previousPoint, point)
            path.addQuadCurve(to: midPoint, controlPoint: self.previousPoint)
            self.display(path: path)
            self.path = UIBezierPath()
            self.path.move(to: midPoint)
            self.pathCount += 1
        }
    }
    public func finish() {
        if self.pathCount == 0 {
            print("pathCount==0")
        }
    }
    
    func display(path: UIBezierPath) {

        self.context.addPath(path.cgPath)
        self.context.strokePath()

//        self.topContext.addPath(path.cgPath)
//        self.bottomContext.addPath(path.cgPath)

        let image = self.context.makeImage()
        self.topLayer.contents = image

    }
    
    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
    }
    

    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
        #if os(Linux)
           
        #elseif os(macOS)
        
        #endif
        
        
        
        Progress.current()
        print("\(function) \(item())")
    }
    
//    open func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive touch: UITouch) -> Bool {
//        print("gestureRecognizer shouldReceive \(gestureRecognizer) \(touch.location(in: gestureRecognizer.view))")
//        return true
//    }
    

}



open class DrawingView: UIView {
    public let panGestureRecognizer: DrawingPanGestureRecognizer = DrawingPanGestureRecognizer()

    public let topLayer: CALayer

    public let context: CGContext
    public init(width: UInt32, height: UInt32) {
        assert(height % 2 == 0)
        let scale = UIScreen.main.scale
        let frame: CGRect = CGRect(x: 0, y: 0, width: CGFloat(width) / scale, height: CGFloat(height) / scale)
        let topLayer: CALayer = CALayer()
        self.topLayer = topLayer;
        
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: Int(width * height * 4), alignment: 128)

        let context = CGContext(data: ptr, width: Int(width), height: Int(height), bitsPerComponent: 8, bytesPerRow: Int(width * 4), space: ColorSpace.deviceRgb, bitmapInfo: ColorSpace.little32Argb.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.context = context
        
        
        context.setLineWidth(2)
        context.setStrokeColor(red: 1, green: 0, blue: 0, alpha: 1)
        context.translateBy(x: 0, y: CGFloat(height))
        context.scaleBy(x: scale, y: -scale)

        super.init(frame: frame)
        topLayer.frame = self.bounds
        
        self.layer.addSublayer(topLayer)
        self.panGestureRecognizer.addTarget(self, action: #selector(DrawingView.handlePan))
//        self.addGestureRecognizer(self.panGestureRecognizer)
        self.panGestureRecognizer.maximumNumberOfTouches = 1
    }
    
    private func midpointOf(_ p0: TouchPoint, _ p1: TouchPoint) -> CGPoint {
        return CGPoint(x: (p0.location.x + p1.location.x) / 2.0, y: (p0.location.y + p1.location.y) / 2.0)
    }
    
    @objc private func handlePan(recognizer: UIPanGestureRecognizer) {
        guard recognizer == self.panGestureRecognizer else {
            return
        }
        let location = recognizer.location(in: recognizer.view)
        let point = TouchPoint(location: location, velocity: recognizer.velocity(in: recognizer.view), time: CACurrentMediaTime())
        print("\(recognizer.state) \(recognizer.state.rawValue) \(location)")
        if recognizer.state == .began {
//            self.begin(point)
        } else if recognizer.state == .changed {
//            self.change(point)
        } else {
//            if recognizer.state == .cancelled || recognizer.state == .ended {
//                self.finish(point)
//            }
        }
        self.setNeedsDisplay()
    }
    
    required public init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    var points: [CGPoint] = []
    
    
    var changedCount: Int = 0
    var repeatCount: Int = 0
    private var previousPoint: CGPoint = CGPoint()

    private func moveIfNeeded(_ point: CGPoint, isLast: Bool) {
        if point == self.previousPoint {
            self.repeatCount += 1
            if self.changedCount > 0 && self.repeatCount < 2 {
                self.move(point, isLast: isLast)
            }
        } else {
            self.move(point, isLast: isLast)
            self.previousPoint = point
            self.repeatCount = 0
            self.changedCount += 1
        }
    }
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        let point = touch.location(in: self)

        print("---[begin\(point)]")


        self.changedCount = 0
        self.repeatCount = 0
        self.previousPoint = point
        self.begin(point)
                
        print("touchesBegan coalescedTouches \(touch) \(event?.coalescedTouches(for: touch))")

    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        print("---[moved\(point)]")
        self.moveIfNeeded(point, isLast: false)
        
        print("touchesMoved coalescedTouches \(event?.coalescedTouches(for: touch))")

    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        print("---[end\(point)]")
        self.moveIfNeeded(point, isLast: true)
        self.finish()
        
        print("touchesEnded coalescedTouches \(event?.coalescedTouches(for: touch))")
        print("predictedTouches \(event?.predictedTouches(for: touch))")

    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else {
            return
        }
        print("---[cancel]")
        let point = touch.location(in: self)
        self.moveIfNeeded(point, isLast: true)
        self.finish()
        
        print("touchesCancelled coalescedTouches \(event?.coalescedTouches(for: touch))")
        print("predictedTouches \(event?.predictedTouches(for: touch))")

    }
    


    public var path: UIBezierPath = UIBezierPath()


    private func midpoint(_ p0: CGPoint, _ p1: CGPoint) -> CGPoint {
        return CGPoint(x: (p0.x + p1.x) / 2.0, y: (p0.y + p1.y) / 2.0)
    }

    var pathCount: Int = 0

    public func begin(_ point: CGPoint) {
        self.path.move(to: point)
        self.pathCount = 0
        
        let path = UIBezierPath()
        path.move(to: point)
        
        var p = point
        p.x += 1
        p.y += 1
        path.addLine(to: p)
        self.display(path: path)
    }
    public func move(_ point: CGPoint, isLast: Bool = false) {
        if isLast {
            self.path.addQuadCurve(to: point, controlPoint: self.previousPoint)
            self.display(path: path)
            self.path = UIBezierPath()
        } else {
            let midPoint = self.midpoint(self.previousPoint, point)
            path.addQuadCurve(to: midPoint, controlPoint: self.previousPoint)
            self.display(path: path)
            self.path = UIBezierPath()
            self.path.move(to: midPoint)
            self.pathCount += 1
        }
    }
    public func finish() {
        if self.pathCount == 0 {
            print("pathCount==0")
        }
    }
    
    func display(path: UIBezierPath) {

        self.context.addPath(path.cgPath)
        self.context.strokePath()

//        self.topContext.addPath(path.cgPath)
//        self.bottomContext.addPath(path.cgPath)

        let image = self.context.makeImage()
        self.topLayer.contents = image

    }
    
    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
    }
    

    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
        #if os(Linux)
           
        #elseif os(macOS)
        
        #endif
        
        
        
        Progress.current()
        print("\(function) \(item())")
    }
    
//    open func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive touch: UITouch) -> Bool {
//        print("gestureRecognizer shouldReceive \(gestureRecognizer) \(touch.location(in: gestureRecognizer.view))")
//        return true
//    }
    

}




class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.view.backgroundColor = UIColor.lightGray
        let v = DrawingView(width: 900, height: 1800)
        v.frame.origin.x = 20
        v.frame.origin.y = 60
        v.backgroundColor = UIColor.white
        self.view.addSubview(v)
//
//        Memory.test()
        let gesture: UITapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(tapped))
//        self.view.addGestureRecognizer(gesture)

//        let v = SamplePointView(frame: CGRect.init(x: 20, y: 60, width: 300, height: 800))
//        v.backgroundColor = UIColor.white
//        self.view.addSubview(v)
    }

    @objc func tapped() {
        print("123")
    }

}

