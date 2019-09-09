//
//  Layout.swift
//  Layout
//
//  Created by vector on 2019/9/8.
//  Copyright © 2019 vector. All rights reserved.
//

import UIKit

public struct LayoutVector: Hashable {
    public static let scale: CGFloat = {
        return UIScreen.main.scale
    }()
    
    public var x: Int32
    public var y: Int32
    public init(x: Int32, y: Int32) {
        self.x = x
        self.y = y
    }
    public static func == (lhs: LayoutVector, rhs: LayoutVector) -> Bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.x)
        hasher.combine(self.y)
    }
    public func transform(scale: CGFloat = LayoutVector.scale) -> (CGFloat, CGFloat) {
        var x = CGFloat(self.x)
        var y = CGFloat(self.y)
        x = x / scale
        y = y / scale
        return (x, y)
    }
    
    public static func +(lhs: LayoutVector, rhs: LayoutVector) -> LayoutVector {
        var result = lhs
        result.x += rhs.x
        result.y += rhs.y
        return result
    }
}

public struct LayoutRect: Hashable {
    public internal(set) var origin: LayoutVector
    public internal(set) var size: LayoutVector
    public static func == (lhs: LayoutRect, rhs: LayoutRect) -> Bool {
        return lhs.origin == rhs.origin && lhs.size == rhs.size
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(origin)
    }
}

//public struct LayoutEdgeInsets: Hashable {
//    public var top: Int32
//    public var left: Int32
//    public var bottom: Int32
//    public var right: Int32
//    public init(top: Int32, left: Int32, bottom: Int32, right: Int32) {
//        self.top = top
//        self.left = left
//        self.bottom = bottom
//        self.right = right
//    }
//    public static func == (lhs: LayoutEdgeInsets, rhs: LayoutEdgeInsets) -> Bool {
//        return lhs.top == rhs.top && lhs.left == rhs.left && lhs.bottom == rhs.bottom && lhs.right == rhs.right
//    }
//    public func hash(into hasher: inout Hasher) {
//        hasher.combine(self.top)
//        hasher.combine(self.left)
//        hasher.combine(self.bottom)
//        hasher.combine(self.right)
//    }
//}


public class LayoutViewBridge: NSObject {

    public enum Style {
        case viewToLayout
        case layoutToView
    }
    
    private final class ViewToLayout: LayoutViewBridge {
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
    
    private final class LayoutToView: LayoutViewBridge {
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
    public override init(origin: LayoutVector, size: LayoutVector) {
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

open class LayoutView: UIView {
    public var contentLayout: Layout? = nil {
        willSet(newValue) {
            
        }
        didSet(oldValue) {
            
        }
    }

    
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
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
public class BaseLayout {
    public weak private(set) var parent: BaseLayout?
    
    //remove from context
    //remove from parent
    
    //add to parent
    //add to context

    
    fileprivate func willMoveToParent(_ newParent: Layout?) {
        self.isParentEnabledInContext = false
    }
    fileprivate func didMoveToParent(_ newParent: Layout?) {
        if let parent = newParent {
            self.isParentEnabledInContext = parent.isEnabledInContext
        } else {
            self.isParentEnabledInContext = false
        }
    }
    fileprivate func didMoveToContext(_ newParent: Layout?) {
        self.parent = newParent
        if let parent = newParent {
            self.isParentEnabledInContext = parent.isEnabledInContext
        } else {
            self.isParentEnabledInContext = false
        }
    }
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
    public internal(set) var origin: LayoutVector {
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
    public internal(set) var originInContext: LayoutVector?  {
        didSet(oldValue) {
            let newValue = self.originInContext
            if oldValue != newValue {
                self.didOriginInContextChanged(from: oldValue, to: newValue)
                self.layoutChildsOriginInContext(newValue)
            }
        }
    }
    public internal(set) var size: LayoutVector {
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
    
    private static let parentDisabledInContextFlag: UInt32 = 0x1
    private static let disabledFlag: UInt32 = 0x2
    private static let removingFlag: UInt32 = 0x4

    private var disableFlag: UInt32 = BaseLayout.parentDisabledInContextFlag {
        willSet(newValue) {
            let oldValue = self.disableFlag
            if oldValue != newValue {
                if oldValue == 0 {
                    self.isEnabledInContextWillChanged(to: true)
                } else if newValue == 0 {
                    self.isEnabledInContextWillChanged(to: false)
                }
            }
        }
        didSet(oldValue) {
            let newValue = self.disableFlag
            if oldValue != newValue {
                if oldValue == 0 {
                    self.isEnabledInContextDidChanged(to: true)
                } else if newValue == 0 {
                    self.isEnabledInContextDidChanged(to: false)
                }
            }
        }
    }
    
    fileprivate(set) var isParentEnabledInContext: Bool {
        get {
            return (self.disableFlag & BaseLayout.parentDisabledInContextFlag == BaseLayout.parentDisabledInContextFlag)
        }
        set(newValue) {
            let oldValue = self.isParentEnabledInContext
            if oldValue != newValue {
                if newValue {
                    self.disableFlag = (self.disableFlag & (~BaseLayout.parentDisabledInContextFlag))
                } else {
                    self.disableFlag = self.disableFlag | BaseLayout.parentDisabledInContextFlag
                }
            }
        }
    }
    
    var isEnabledInContext: Bool {
        get {
            return (self.disableFlag == 0)
        }
    }
    
    public var isEnabled: Bool {
        get {
            return (self.disableFlag & BaseLayout.disabledFlag == BaseLayout.disabledFlag)
        }
        set(newValue) {
            let oldValue = self.isEnabled
            if oldValue != newValue {
                if newValue {
                    self.disableFlag = (self.disableFlag & (~BaseLayout.disabledFlag))
                } else {
                    self.disableFlag = self.disableFlag | BaseLayout.disabledFlag
                }
            }
        }
//        didSet(oldValue) {
//            let newValue = self.isEnabled
//            if oldValue != newValue {
//                self.isEnabledInContext = self.isParentEnabledInContext && self.isEnabled
//                if let parent = self.parent {
//                    parent.didChildIsEnabledChanged(self, from: oldValue, to: newValue)
//                }
//            }
//        }
    }

    public init(origin: LayoutVector, size: LayoutVector) {
        self.origin = origin
        self.size = size
    }
    public static func == (lhs: BaseLayout, rhs: BaseLayout) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        let objPtr = Unmanaged.passUnretained(self).toOpaque()
        hasher.combine(objPtr)
    }
    
    public var currentContext: LayoutContext? {
        return parent?.currentContext
    }
    
    internal func clearOriginInContext() {
        print("clearOriginInContext")
        self.originInContext = nil
    }
    internal func layoutOriginInContext(parentOriginInContext: LayoutVector) {
        print("layoutOriginInContext")
        self.originInContext = parentOriginInContext + self.origin
    }
    
    //self changed
    internal func didParentChanged(to: BaseLayout?) {
        print("didIsEnabledChanged")
    }
    internal func isEnabledInContextWillChanged(to: Bool) {
        if !to {
            //remove from context
        }
        print("isEnabledInContextWillChanged")
    }
    internal func isEnabledInContextDidChanged(to: Bool) {
        print("isEnabledInContextDidChanged")
        if to {
            //add to context
        }
    }
    internal func didOriginInContextChanged(from: LayoutVector?, to: LayoutVector?) {
        print("didOriginInContextChanged")
    }
    internal func didOriginChanged(from: LayoutVector, to: LayoutVector) {
        print("didOriginChanged")
    }
    internal func didSizeChanged(from: LayoutVector, to: LayoutVector) {
        print("didOriginChanged")
    }
    
    //child changed

    internal func didChildIsEnabledChanged(_ child: BaseLayout, from: Bool, to: Bool) {
        print("didChildIsEnabledChanged")
    }
    internal func didChildOriginChanged(_ child: BaseLayout, from: LayoutVector, to: LayoutVector) {
        print("didChildOriginChanged")
    }
    internal func didChildSizeChanged(_ child: BaseLayout, from: LayoutVector, to: LayoutVector) {
        print("didChildOriginChanged")
    }
    
    internal func willMove(to context: LayoutContext?) {
        print("willMove")
    }
    
    internal func didMoveToContext() {

    }
    
    //子类实现
    
    internal func clearChildsOriginInContext() {
        print("clearChildsOriginInContext")
    }
    internal func layoutChildsOriginInContext(_ originInContext: LayoutVector?) {
        print("layoutChildsOriginInContext")
    }
    
}

public class Layout: BaseLayout {
    public var view: UIView? = nil {
        didSet {
            self.resetViewFrame()
        }
    }
    
    public override var originInContext: LayoutVector?  {
        didSet {
            self.resetViewFrame()
        }
    }
    
    
    private func viewFrame(origin: LayoutVector, size: LayoutVector) -> CGRect {
        let (x, y) = origin.transform()
        let (width, height) = size.transform()
        let frame: CGRect = CGRect(x: x, y: y, width: width, height: height)
        return frame
    }
    private func resetViewFrame() {
        guard let viewOrigin = self.originInContext else {
            return
        }
        guard let view = self.view else {
            return
        }
        view.frame = self.viewFrame(origin: viewOrigin, size: self.size)
    }
    
    internal override func didSizeChanged(from: LayoutVector, to: LayoutVector) {
        super.didSizeChanged(from: from, to: to)
        self.resetViewFrame()
    }
    
//    public override init(origin: LayoutVector, size: LayoutVector) {
//        super.init(origin: origin, size: size)
//    }
    
    internal override func isEnabledInContextDidChanged(to: Bool) {
        super.isEnabledInContextDidChanged(to: to)

        guard let view = self.view else {
            return
        }
        if to {
            if let contentView = self.currentContext?.view {
                contentView.addSubview(view)
            }
        } else {
            view.removeFromSuperview()
        }
    }
    
    
    
}
public class LayoutGroup: BaseLayout {
    public override init(origin: LayoutVector, size: LayoutVector) {
        super.init(origin: origin, size: size)
        self.isEnabled = false
        self.isEnabled = false

    }

}

public class LinearLayout: Layout {

    
    
    
    
    
    
}
