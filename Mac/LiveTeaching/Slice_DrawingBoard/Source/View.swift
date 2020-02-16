//
//  View.swift
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

open class DisplayLayer: Hashable {
    public static func == (lhs: DisplayLayer, rhs: DisplayLayer) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
    public private(set) var sublayers: [DisplayLayer] = []
    public private(set) unowned var superlayer: DisplayLayer?
    
    var _origin: Point
    var _size: Size
    
    public var origin: Point {
        set {
            _origin = newValue
        }
        get {
            return _origin
        }
    }
    public var size: Size {
        set {
            _size = newValue
        }
        get {
            return _size
        }
    }
    
    
    public init(origin: Point, size: Size) {
        _origin = origin
        _size = size
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
    
    var _origin: Point
    var _size: Size
    
    public var origin: Point {
        set {
            _origin = newValue
        }
        get {
            return _origin
        }
    }
    public var size: Size {
        set {
            _size = newValue
        }
        get {
            return _size
        }
    }
    
    
    public init(origin: Point, size: Size) {
        _origin = origin
        _size = size
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
