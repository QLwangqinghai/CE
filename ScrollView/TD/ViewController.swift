//
//  ViewController.swift
//  TD
//
//  Created by vector on 2020/1/5.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


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

open class DrawingView: UIView {
    public let topLayer: CALayer
    public let bottomLayer: CALayer

    public let topContext: CGContext
    public let bottomContext: CGContext

    public init(width: UInt32, height: UInt32) {
        assert(height % 2 == 0)
        let scale = UIScreen.main.scale
        let frame: CGRect = CGRect(x: 0, y: 0, width: CGFloat(width) / scale, height: CGFloat(height) / scale)
        let topLayer: CALayer = CALayer()
        let bottomLayer: CALayer = CALayer()
        self.topLayer = topLayer;
        self.bottomLayer = bottomLayer
        
        
        let top = UnsafeMutableRawPointer.allocate(byteCount: Int(width * height * 2), alignment: 128)
        let bottom = UnsafeMutableRawPointer.allocate(byteCount: Int(width * height * 2), alignment: 128)

        let topContext = CGContext(data: top, width: Int(width), height: Int(height/2), bitsPerComponent: 8, bytesPerRow: Int(width * 4), space: ColorSpace.deviceRgb, bitmapInfo: ColorSpace.little32Argb.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.topContext = topContext
        let bottomContext = CGContext(data: bottom, width: Int(width), height: Int(height/2), bitsPerComponent: 8, bytesPerRow: Int(width * 4), space: ColorSpace.deviceRgb, bitmapInfo: ColorSpace.little32Argb.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.bottomContext = bottomContext
        
        
        topContext.setLineWidth(3)
        topContext.setStrokeColor(red: 1, green: 0, blue: 0, alpha: 1)
        topContext.translateBy(x: 0, y: CGFloat(height/2))
        topContext.scaleBy(x: 1.0, y: -1.0)

        bottomContext.setLineWidth(3)
        bottomContext.setStrokeColor(red: 1, green: 0, blue: 0, alpha: 1)
//        bottomContext.setStrokeColor(red: 0, green: 0, blue: 1, alpha: 1)
        bottomContext.translateBy(x: 0, y: CGFloat(height/2))
        bottomContext.scaleBy(x: 1.0, y: -1.0)
        bottomContext.translateBy(x: 0, y: (-1.0) * CGFloat(height/2))

        super.init(frame: frame)
        var bounds = self.bounds
        bounds.size.height = bounds.height / 2
        topLayer.frame = bounds
        bounds.origin.y += bounds.height
        bottomLayer.frame = bounds
        
        self.layer.addSublayer(topLayer)
        self.layer.addSublayer(bottomLayer)
    }
    
    required public init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    var points: [CGPoint] = []
    
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch: UITouch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        self.points.append(point)
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch: UITouch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        self.points.append(point)

    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch: UITouch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        self.points.append(point)
        self.finish()
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch: UITouch = touches.first else {
            return
        }
        let point = touch.location(in: self)
        self.points.append(point)
        self.finish()
    }
    
    func finish() {
        let points: [CGPoint] = self.points
        self.points.removeAll()
        
        let path = UIBezierPath()
        path.lineWidth = 2
        
        let scale = UIScreen.main.scale
        for (index, point) in points.enumerated() {
            let p = CGPoint(x: point.x * scale, y: point.y * scale)
            if index == 0 {
                self.topContext.move(to: p)
                self.bottomContext.move(to: p)
            } else {
                self.topContext.addLine(to: p)
                self.bottomContext.addLine(to: p)
            }
        }
        self.topContext.strokePath()
        self.bottomContext.strokePath()
        
        self.bottomContext.fill(CGRect(x: 30, y: 30, width: 30, height: 30))
        self.bottomContext.setFillColor(red: 0, green: 0, blue: 0, alpha: 1)
        
//        self.topContext.addPath(path.cgPath)
//        self.bottomContext.addPath(path.cgPath)

        let image = self.topContext.makeImage()
        self.topLayer.contents = image
        self.bottomLayer.contents = self.bottomContext.makeImage()

    }
    
    open func logTouch(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(item: "\(touches) \(String(describing: event))")
    }
    
    func log(item: @autoclosure () -> Any, _ file: StaticString = #file, _ line: Int = #line, _ function: String = #function) {
//        print("\(file) :\(line): \(function) \(item())")
        
        print("\(function) \(item())")
    }
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
    }


}

