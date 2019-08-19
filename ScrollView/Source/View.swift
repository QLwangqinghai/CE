//
//  View.swift
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public typealias Rect = C2DRect
public typealias Size = C2DSize
public typealias Point = C2DPoint

public typealias Rect64 = C2DRect64
public typealias Size64 = C2DSize64
public typealias Point64 = C2DPoint64

open class DisplayLayer: Hashable {
    public static func == (lhs: DisplayLayer, rhs: DisplayLayer) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
    public private(set) var sublayers: [DisplayLayer] = []
    public private(set) unowned var superlayer: DisplayLayer?
    public var frame: Rect
    
    public init(frame: Rect) {
        self.frame = frame
    }
    
//    private func __addSubview(_ subview: View) {
//        subviews.append(subview)
//        subview.superview = self
//    }
//    
//    open func addSublayper(_ subview: View) {
//        subview.removeFromSuperview()
//        __addSubview(subview)
//    }
//    
//    open func removeSubview(_ view: View) {
//        if view.superview == self {
//            self.subviews.removeAll(where: { (item) -> Bool in
//                return (item == self)
//            })
//            view.superview = nil
//        }
//    }
//    
//    open func removeFromSuperview() {
//        self.superview?.removeSubview(self)
//    }
    
}



open class View: Responder {
    public private(set) var subviews: [View] = []
    public private(set) unowned var superview: View?
    public var frame: Rect
    
    public init(frame: Rect) {
        self.frame = frame
        super.init()
    }
    
    private func __addSubview(_ subview: View) {
        subviews.append(subview)
        subview.superview = self
    }
    
    open func addSubview(_ subview: View) {
        subview.removeFromSuperview()
        __addSubview(subview)
    }
    
    open func removeSubview(_ view: View) {
        if view.superview == self {
            self.subviews.removeAll(where: { (item) -> Bool in
                return (item == self)
            })
            view.superview = nil
        }
    }
    
    open func removeFromSuperview() {
        self.superview?.removeSubview(self)
    }
    
}


open class MV : UIView {
    
    open override func draw(_ layer: CALayer, in ctx: CGContext) {
//        layer.superlayer
    }
    
    open override func draw(_ rect: CGRect) {
        
    }
    
}
