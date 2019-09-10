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

public class ViewContentLayout: Layout {
    public init(origin: LayoutVector, size: LayoutVector) {
        let context: LayoutContext = LayoutContext()
        super.init(origin: origin, size: size, flag: .none, context: context)
        context.didViewChanged = {[weak self](context: LayoutContext) -> Void in
            guard let `self` = self else {
                return
            }
            self.didContextViewChanged()
        }
    }
    deinit {
        self.context?.didViewChanged = nil
    }
    private func didContextViewChanged() {
        
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
public struct LayoutFlag: OptionSet {
    public var rawValue: UInt32
    
    public init(rawValue: UInt32) {
        self.rawValue = rawValue
    }
    
    static let groupable: LayoutFlag = LayoutFlag(rawValue: 1 << 0)
    static let none: LayoutFlag = []
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
public class BaseLayout {
    
    public weak private(set) var parent: LayoutGroup?
    
    //disable in context
    //remove from context
    //clear context
    //clear parent
    //remove from parent
    
    //add to parent
    //check enabled in context
    //add to context
    
    fileprivate func _removeFromParent() {
        guard let parent = self.parent else {
            return
        }
        parent.willRemoveChild(self)
        self.willMoveToContext(nil)
        self.willMoveToParent(nil)
        self.context = nil
        self.parent = nil
        parent._clearStore(child)
        parent.didRemoveChild(child)
        self.didMoveToParent(nil)
        self.didMoveToContext(nil)
    }
    
    internal func willRemoveChild(_ child: BaseLayout) {
        
        
    }
    internal func didRemoveChild(_ child: BaseLayout) {
        
        
    }
    internal func willAddChild(_ child: BaseLayout) {
        
        
    }
    internal func didAddChild(_ child: BaseLayout) {
        
        
    }
    
    internal func willMoveToParent(_ parent: LayoutGroup?) {
        
        
    }
    internal func didMoveToParent() {
        
        
    }
    
    internal func willMoveToContext(_ context: LayoutContext?) {
    }
    internal func didMoveToContext() {
    }
    
    private var contextStorage: ContextStorage  {
        willSet(newValue) {
            let oldValue = self.contextStorage
            if oldValue.childContext != newValue.childContext {
                
            }
        }
        didSet(oldValue) {
            let newValue = self.contextStorage
            if oldValue.childContext != newValue.childContext {
                
            }
        }
    }
    
    public var childContext: LayoutContext? {
        return self.context
    }

    public fileprivate(set) var context: LayoutContext? = nil
//    {
//        willSet(newValue) {
//            let oldValue = self.context
//            if oldValue != newValue {
//
//            }
//        }
//        didSet(oldValue) {
//            let newValue = self.context
//            if oldValue != newValue {
//
//            }
//        }
//    }
    
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
    private static let disabledFlag: UInt32 = 0x1 << 1
    private static let movingFlag: UInt32 = 0x1 << 31

    fileprivate(set) var isMoving: Bool {
        get {
            return (self.disableFlag & BaseLayout.movingFlag == BaseLayout.movingFlag)
        }
        set(newValue) {
            let oldValue = self.isMoving
            if oldValue != newValue {
                if newValue {
                    self.disableFlag = self.disableFlag | BaseLayout.movingFlag
                } else {
                    self.disableFlag = (self.disableFlag & (~BaseLayout.movingFlag))
                }
            }
        }
    }
    
    private var flag: UInt32
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

    public init(origin: LayoutVector, size: LayoutVector, flag: LayoutFlag, context: LayoutContext? = nil) {
        self.origin = origin
        self.size = size
        self.contextStorage = ContextStorage(current: context)
        self.flag = flag.rawValue
    }
    public static func == (lhs: BaseLayout, rhs: BaseLayout) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        let objPtr = Unmanaged.passUnretained(self).toOpaque()
        hasher.combine(objPtr)
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
        self.noticeChildsIsParentEnabledInContextWillChanged(to)
        if !to {
            //self remove from context
        }
        print("isEnabledInContextWillChanged")
    }
    internal func isEnabledInContextDidChanged(to: Bool) {
        print("isEnabledInContextDidChanged")
        if to {
            //add to context
        }
        self.noticeChildsIsParentEnabledInContextDidChanged(to)
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
    internal func noticeChildsIsParentEnabledInContextWillChanged(to: Bool) {
        
    }
    internal func noticeChildsIsParentEnabledInContextDidChanged(to: Bool) {
        
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
    public var _childContext: LayoutContext?

    private func getChildContext(context: LayoutContext?, childContext: LayoutContext?) {
        return childContext ?? context
    }
    
    public override var context: LayoutContext? = nil {
        willSet(newValue) {
            let oldValue = self.context
            if oldValue != newValue {
                let oldChildContext = self.getChildContext(context: oldValue, childContext: self._childContext)
                let newChildContext = self.getChildContext(context: newValue, childContext: self._childContext)
                if oldChildContext != newChildContext {
                    self.childContextWillChanged(to: newChildContext)
                }
            }
        }
        didSet(oldValue) {
            let newValue = self.context
            if oldValue != newValue {
                let oldChildContext = self._childContext ?? oldValue
                let newChildContext = self._childContext ?? newValue
                if oldChildContext != newChildContext {
                    self.childContextDidChanged(to: newChildContext)
                }
            }
        }
    }
        
        
    public override var childContext: LayoutContext? {
        return self._childContext ?? self.context
    }

    internal private(set) var childs: [BaseLayout] = []
    
    fileprivate func _toStore(child: BaseLayout) {
        self.childs.append(child)
    }
    fileprivate func _clearStore(child: BaseLayout) {
        self.childs.remove(child)
    }
    fileprivate func _addChild(child: BaseLayout) {
        child._removeFromParent()
        
        if let context = self.context {
            child.willMoveToContext(context)
        }
        self.willAddChild(child)
        child.willMoveToParent(self)
        self._toStore(child)
        child.parent = self
        child.context = self.context
        child.didMoveToParent(self)
        self.didAddChild(child)
        if let context = self.context {
            child.didMoveToContext(context)
        }
    }
    
    public override init(origin: LayoutVector, size: LayoutVector) {
        super.init(origin: origin, size: size)
        self.isEnabled = false
        self.isEnabled = false

    }

    internal func childContextWillChanged(to: LayoutContext?) {
        
    }
    internal func childContextDidChanged(to: LayoutContext?) {
        
    }
}

public class LinearLayout: Layout {

    
    
    
    
    
    
}
