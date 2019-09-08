//
//  Layout.swift
//  Layout
//
//  Created by vector on 2019/9/8.
//  Copyright © 2019 vector. All rights reserved.
//

import UIKit

public struct LayoutPoint: Hashable {
    public var x: Int32
    public var y: Int32
    public init(x: Int32, y: Int32) {
        self.x = x
        self.y = y
    }
    public static func == (lhs: LayoutPoint, rhs: LayoutPoint) -> Bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.x)
        hasher.combine(self.y)
    }
}
public struct LayoutSize: Hashable {
    public var width: Int32
    public var height: Int32
    public init(width: Int32, height: Int32) {
        self.width = width
        self.height = height
    }
    public static func == (lhs: LayoutSize, rhs: LayoutSize) -> Bool {
        return lhs.width == rhs.width && lhs.height == rhs.height
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.width)
        hasher.combine(self.height)
    }
}
public struct LayoutEdgeInsets: Hashable {
    public var top: Int32
    public var left: Int32
    public var bottom: Int32
    public var right: Int32
    public init(top: Int32, left: Int32, bottom: Int32, right: Int32) {
        self.top = top
        self.left = left
        self.bottom = bottom
        self.right = right
    }
    public static func == (lhs: LayoutEdgeInsets, rhs: LayoutEdgeInsets) -> Bool {
        return lhs.top == rhs.top && lhs.left == rhs.left && lhs.bottom == rhs.bottom && lhs.right == rhs.right
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.top)
        hasher.combine(self.left)
        hasher.combine(self.bottom)
        hasher.combine(self.right)
    }
}


//public struct LayoutRect: Hashable {
//    public var x: Int
//    public var y: Int
//    public var height: Int
//    public var width: Int
//    public init() {
//    }
//    public static func == (lhs: LayoutRect, rhs: LayoutRect) -> Bool {
//        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height
//    }
//    public func hash(into hasher: inout Hasher) {
//        hasher.combine(x)
//        hasher.combine(y)
//    }
//}





public class LayoutViewBridge: NSObject {

    public enum Style {
        case viewToLayout
        case layoutToView
    }
    
    private class ViewToLayout: LayoutViewBridge {
        private static var observerContextValue: Int = 0
        public let _layout: ViewContentLayout

        public override var view: UIView? {
            willSet {
                if let view = self.view {
                    view.removeObserver(self, forKeyPath: #keyPath(UIView.frame), context: &ViewToLayout.observerContextValue)
                }
            }
            didSet {
                self._layout.context.view = self.view
                if let view = self.view {
                    view.addObserver(self, forKeyPath: #keyPath(UIView.frame), context: &ViewToLayout.observerContextValue)
                }
            }
        }
        
        fileprivate init(layout: ViewContentLayout) {
            self._layout = layout
            super.init(layout: layout, style: .viewToLayout)
        }
        
        deinit {
            self.view = nil
        }
        
        override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
            if context == &ViewToLayout.observerContextValue {
                if let path = keyPath {
                    switch path {
                    case #keyPath(UIView.frame):
                        
                        
                        break
                    default:
                        break
                    }
                }
            } else {
                super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
            }
        }
    }
    
    private class LayoutToView: LayoutViewBridge {
        private static var observerContextValue: Int = 0
        
        public override var view: UIView? {
            didSet {
                if let view = self.view {
                }
            }
        }
        
        fileprivate init(layout: Layout) {
            super.init(layout: layout, style: .layoutToView)
        }
    }

    public let layout: Layout
    public let style: Style

    public fileprivate(set) var view: UIView?
    
    fileprivate init(layout: Layout, style: Style) {
        self.layout = layout
        self.style = style
        self.view = nil
    }
    
    public func bind(view: UIView?) {
        if view != self.view {
            self.view = view

//            if self.view != nil && view != nil {
//                self.view = nil
//                self.view = view
//            } else {
//                self.view = view
//            }
        }
    }
    
    public static func viewContentLayout(layout: ViewContentLayout) -> LayoutViewBridge {
        return ViewToLayout(layout: layout)
    }
    
    public static func ViewLayout(layout: Layout) -> LayoutViewBridge {
        return LayoutToView(layout: layout)
    }
    
}



public class ViewContentLayout: Layout {
    fileprivate let context: LayoutContext
    public override var currentContext: LayoutContext? {
        return self.context
    }
    public override init(origin: LayoutPoint, size: LayoutSize) {
        self.context = LayoutContext()
        super.init(origin: origin, size: size)
        self.context.didViewChanged = {[weak self](context: LayoutContext) -> Void in
            guard let `self` = self else {
                return
            }
            self.didContextChanged()
        }
    }
    deinit {
        self.context.didViewChanged = nil
    }
    private func didContextChanged() {
        
    }
}


public final class LayoutContext {
    fileprivate(set) unowned var view: UIView?
//    {
//        didSet {
//            if let value = view {
//                let objPtr = Unmanaged.passUnretained(self).toOpaque()
//                let address: Int = objPtr.load(as:Int.self)
//                self.viewIdentifier = address
//            } else {
//                self.viewIdentifier = 0
//            }
//        }
//    }
    fileprivate var didViewChanged: ((LayoutContext) -> Void)?
//    fileprivate private(set) var viewIdentifier: Int

    
    public init() {
        self.view = nil
        self.didViewChanged = nil
//        self.viewIdentifier = 0
    }
    
}
public class Layout {
    public weak private(set) var parent: Layout?
    
    //remove from context
    //remove from parent
    
    //add to parent
    //add to context
    
    
//    fileprivate func updateParent(_ newParent: Layout?) {
//        let currentParent: Layout? = self.parent
//        if let to = newParent {
//            if let from = currentParent {
//                self.parent = to
//            } else {
//                self.parent = to
//            }
//        } else {
//            if currentParent == nil {
//                return
//            } else {
//                self.parent = nil
//            }
//        }
//    }
//
//    fileprivate func unbind(child: Layout) {
//        if let p = child.parent {
//            if p == self {
//                child.updateParent(nil)
//            } else {
//                print("child:\(child) is not self(\(self)).sub")
//            }
//        } else {
//            print("child:\(child) is not in super")
//        }
//    }
//    fileprivate func bind(child: Layout) {
//        if let p = child.parent {
//            if p == self {
//                print("child:\(child) been in super(\(p))")
//                return
//            } else {
//                print("child:\(child) is in super(\(p))")
//                p.unbind(child: child)
//            }
//        }
//        //add
//        child.updateParent(parent)
//    }
    public internal(set) var origin: LayoutPoint {
        didSet(oldValue) {
            let newValue = self.origin
            if oldValue != newValue {
                self.didOriginChanged(from: oldValue, to: newValue)
                if let parent = self.parent {
                    parent.didChildOriginChanged(self, from: oldValue, to: newValue)
                }
            }
        }
    }
    public internal(set) var size: LayoutSize {
        didSet(oldValue) {
            let newValue = self.size
            if oldValue != newValue {
                self.didSizeChanged(from: oldValue, to: newValue)
                if let parent = self.parent {
                    parent.didChildSizeChanged(self, from: oldValue, to: newValue)
                }
            }
        }
    }
    public var isEnabled: Bool = true {
        didSet(oldValue) {
            let newValue = self.isEnabled
            if oldValue != newValue {
                self.didIsEnabledChanged(from: oldValue, to: newValue)
                if let parent = self.parent {
                    parent.didChildIsEnabledChanged(self, from: oldValue, to: newValue)
                }
            }
        }
    }

    public init(origin: LayoutPoint, size: LayoutSize) {
        self.origin = origin
        self.size = size
    }
    public static func == (lhs: Layout, rhs: Layout) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        let objPtr = Unmanaged.passUnretained(self).toOpaque()
        hasher.combine(objPtr)
    }
    
    public var currentContext: LayoutContext? {
        return parent?.currentContext
    }
    
    //self changed
    internal func didParentChanged(to: Layout?) {
        print("didIsEnabledChanged")
    }
    internal func didIsEnabledChanged(from: Bool, to: Bool) {
        print("didIsEnabledChanged")
    }
    internal func didOriginChanged(from: LayoutPoint, to: LayoutPoint) {
        print("didOriginChanged")
    }
    internal func didSizeChanged(from: LayoutSize, to: LayoutSize) {
        print("didOriginChanged")
    }
    
    //child changed

    internal func didChildIsEnabledChanged(_ child: Layout, from: Bool, to: Bool) {
        print("didChildIsEnabledChanged")
    }
    internal func didChildOriginChanged(_ child: Layout, from: LayoutPoint, to: LayoutPoint) {
        print("didChildOriginChanged")
    }
    internal func didChildSizeChanged(_ child: Layout, from: LayoutSize, to: LayoutSize) {
        print("didChildOriginChanged")
    }
    
    internal func willMove(to context: LayoutContext?) {
        print("willMove")
    }
    
    internal func didMoveToContext() {

    }
    
    
}

public class LayoutGroup: Layout {
    public override init(origin: LayoutPoint, size: LayoutSize) {
        super.init(origin: origin, size: size)
        self.isEnabled = false
        self.isEnabled = false

    }

}

public class LinearLayout: Layout {

    
    
    
    
    
    
}
