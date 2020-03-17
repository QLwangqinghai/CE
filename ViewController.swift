//
//  ViewController.swift
//  T
//
//  Created by vector on 2020/3/13.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

public final class MyObject {
    
    init() {
        
    }
    deinit {
        print("deinit")
    }
}

public final class BitmapInfo: Hashable {
    public static let colorSpace: CGColorSpace = CGColorSpaceCreateDeviceRGB()
    
    public static let littleArgb8888: BitmapInfo = BitmapInfo(bitsPerComponent: 8, bitsPerPixel: 32, bytesPerPixel: 4, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.premultipliedFirst, pixelFormatInfo: CGImagePixelFormatInfo.packed, orderInfo: CGImageByteOrderInfo.order32Little)
    
    public static let littleXrgb555: BitmapInfo = BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst, pixelFormatInfo: CGImagePixelFormatInfo.RGB555, orderInfo: CGImageByteOrderInfo.order16Little)
    
    public static let littleRgb565: BitmapInfo = BitmapInfo(bitsPerComponent: 5, bitsPerPixel: 16, bytesPerPixel: 2, space: BitmapInfo.colorSpace, alphaInfo: CGImageAlphaInfo.noneSkipFirst, pixelFormatInfo: CGImagePixelFormatInfo.RGB565, orderInfo: CGImageByteOrderInfo.order16Little)

    
    public let bitsPerComponent: Int32
    public let bitsPerPixel: Int32
    public let bytesPerPixel: Int32
    public let space: CGColorSpace

    public let alphaInfo: CGImageAlphaInfo
    public let pixelFormatInfo: CGImagePixelFormatInfo
    public let orderInfo: CGImageByteOrderInfo
    
    public var bitmapInfo: UInt32 {
        return self.alphaInfo.rawValue | self.orderInfo.rawValue | self.pixelFormatInfo.rawValue
    }
    
    fileprivate init(bitsPerComponent: Int32, bitsPerPixel: Int32, bytesPerPixel: Int32, space: CGColorSpace, alphaInfo: CGImageAlphaInfo, pixelFormatInfo: CGImagePixelFormatInfo, orderInfo: CGImageByteOrderInfo) {
        self.bitsPerComponent = bitsPerComponent
        self.bitsPerPixel = bitsPerPixel
        self.bytesPerPixel = bytesPerPixel
        self.space = space
        self.alphaInfo = alphaInfo
        self.pixelFormatInfo = pixelFormatInfo
        self.orderInfo = orderInfo
    }
    
    public static func == (lhs: BitmapInfo, rhs: BitmapInfo) -> Bool {
        return lhs.bitsPerComponent == rhs.bitsPerComponent && lhs.bitsPerPixel == rhs.bitsPerPixel && lhs.space == rhs.space && lhs.alphaInfo == rhs.alphaInfo && lhs.pixelFormatInfo == rhs.pixelFormatInfo && lhs.orderInfo == rhs.orderInfo
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.bitsPerComponent)
        hasher.combine(self.bitsPerPixel)
    }
    
    
    public func createBitmap(width: Int, height: Int)-> CGContext? {
        print("bitmapInfo: \(self.bitmapInfo)")
        return CGContext(data: nil, width: width, height: height, bitsPerComponent: Int(self.bitsPerComponent), bytesPerRow: Int(self.bitsPerPixel / 8) * width, space: self.space, bitmapInfo: self.bitmapInfo)
    }
}


class ViewController: UIViewController, CALayerDelegate {

    
    func log(context: CGContext?) {
        if let context = context {
            print("bitsPerPixel: \(context.bitsPerPixel)  bitsPerComponent:\(context.bitsPerComponent) \(context.alphaInfo.rawValue) \(context.bitmapInfo.rawValue)")
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
//        let a = BitmapInfo.littleArgb8888.createBitmap(width: 100, height: 100)
//        let b = BitmapInfo.littleXrgb555.createBitmap(width: 100, height: 100)
//        let c = BitmapInfo.littleRgb565.createBitmap(width: 100, height: 100)
//
//
//        self.log(context: a)
//        self.log(context: b)
//        self.log(context: c)
        
        let obj: MyObject = MyObject()
        let bufferPointer: Unmanaged<MyObject> = Unmanaged.passRetained(obj)

        let v = UIView(frame: CGRect(x: 100, y: 100, width: 50, height: 200))
        self.view.addSubview(v)
        
        
        v.center = CGPoint(x: 125, y: 200)
        
        print(v)
        
        v.center = CGPoint(x: 150, y: 200)

        print(v)

        v.backgroundColor = UIColor(red: 0, green: 1, blue: 0, alpha: 0.5)
        let t = CGAffineTransform.identity.rotated(by: CGFloat(M_PI) / 8)
        v.transform = t
        
        var bounds = UIScreen.main.bounds
        bounds.size.width *= 4
        bounds.size.height *= 4
        
        let layer = CATiledLayer()
        layer.contentsScale = UIScreen.main.scale
        layer.delegate = self
        layer.frame = bounds
        
        let scrollView = UIScrollView(frame: UIScreen.main.bounds)
        scrollView.backgroundColor = .white
        scrollView.contentSize = bounds.size
        self.view.addSubview(scrollView)
        
        scrollView.layer.addSublayer(layer)
        
        layer.setNeedsDisplay()
    }

    func draw(_ layer: CALayer, in ctx: CGContext) {
        let bounds = ctx.boundingBoxOfClipPath
        var red = CGFloat(drand48())
        var green = CGFloat(drand48())
        var blue = CGFloat(drand48())
        ctx.setFillColor(UIColor(red: red, green: green, blue: blue, alpha: 1).cgColor)
        ctx.fill(bounds)
        
        print("draw(_ layer  \(bounds)")
    }
    
}

