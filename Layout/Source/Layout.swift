//
//  Layout.swift
//  Layout
//
//  Created by vector on 2019/9/8.
//  Copyright © 2019 vector. All rights reserved.
//

import UIKit


public typealias LayoutVector = CLVector_t
extension LayoutVector: Hashable {
    public static func == (lhs: LayoutVector, rhs: LayoutVector) -> Bool {
        return CLVectorEqualToVector(lhs, rhs)
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.x)
        hasher.combine(self.y)
    }
}
public extension LayoutVector {
    static let scale: CGFloat = {
        return UIScreen.main.scale
    }()
    func transform(scale: CGFloat = LayoutVector.scale) -> (CGFloat, CGFloat) {
        var x = CGFloat(self.x)
        var y = CGFloat(self.y)
        x = x / scale
        y = y / scale
        return (x, y)
    }
    static func +(lhs: LayoutVector, rhs: LayoutVector) -> LayoutVector {
        var result = lhs
        result.x += rhs.x
        result.y += rhs.y
        return result
    }
}

public typealias LayoutRect = CLRect_t

extension LayoutRect: Hashable {
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

//public class ViewContentLayout: Layout {
//    public init(origin: LayoutVector, size: LayoutVector) {
//        let context: LayoutContext = LayoutContext()
//        super.init(origin: origin, size: size, flag: .none, context: context)
//        context.didViewChanged = {[weak self](context: LayoutContext) -> Void in
//            guard let `self` = self else {
//                return
//            }
//            self.didContextViewChanged()
//        }
//    }
//    deinit {
//        self.context?.didViewChanged = nil
//    }
//    private func didContextViewChanged() {
//
//    }
//}
//
//open class LayoutView: UIView {
//    public var contentLayout: Layout? = nil {
//        willSet(newValue) {
//
//        }
//        didSet(oldValue) {
//
//        }
//    }
//}


//-----------------------------------------------------------------------------------------------------------------------------------------------------

public final class LayoutContextEntity: Hashable {
    
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
    
    fileprivate var viewWillChange: ((_ context: LayoutContext, _ from: UIView?, _ to: UIView?) -> Void)?
    
    fileprivate var viewDidChange: ((_ context: LayoutContext, _ from: UIView?, _ to: UIView?) -> Void)?
    //    fileprivate private(set) var viewIdentifier: Int
    
    
    internal init() {
        self.view = nil
        self.viewWillChange = nil
        self.viewDidChange = nil
    }
    
    public static func == (lhs: LayoutContextEntity, rhs: LayoutContextEntity) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
}


public struct LayoutContext: Equatable {

    fileprivate var entity: LayoutContextEntity
    public internal(set) var origin: LayoutVector

    internal init(entity: LayoutContextEntity, origin: LayoutVector) {
        self.entity = entity
        self.origin = origin
    }
    
    public static func == (lhs: LayoutContext, rhs: LayoutContext) -> Bool {
        return lhs.entity == rhs.entity && lhs.origin == rhs.origin
    }
}

fileprivate class LayoutPool {
    fileprivate static var pools: [LayoutPool] = []

    fileprivate static func push() -> LayoutPool {
        
    }
    fileprivate static func pop() -> LayoutPool {
        
    }

    fileprivate var items: NSPointerArray = NSPointerArray(options: NSPointerFunctions.Options.weakMemory)

    fileprivate func append(layout: Layout) {
        let ptrptr = UnsafeRawPointer(&objPtr)
        let value: UInt = ptrptr.load(as: UInt.self)

        
        
        self.items.addPointer(Unmanaged.passUnretained(self).toOpaque())
    }
    
}


public class Layout: Hashable {
    
    public static func == (lhs: Layout, rhs: Layout) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
    
    public static func layout(_ closure: () -> ()) {
        
    }
    
    public weak fileprivate(set) var parent: LayoutGroup?
    
    public var view: UIView? = nil {
        didSet(oldValue) {
            let newValue = self.view
            if oldValue != newValue {
                self.resetViewFrame()
            }
        }
    }

    private func viewFrame(origin: LayoutVector, size: LayoutVector) -> CGRect {
        let (x, y) = origin.transform()
        let (width, height) = size.transform()
        let frame: CGRect = CGRect(x: x, y: y, width: width, height: height)
        return frame
    }
    private func resetViewFrame() {
        guard let context = self.context else {
            return
        }
        guard let view = self.view else {
            return
        }
        view.frame = self.viewFrame(origin: context.origin + self.origin, size: self.size)
    }
    
    
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
        self.isDisabledByParent = true
        self.context = nil
        self.parent = nil
        parent._clearStore(child: self)
        parent.didRemoveChild(self)
        self.didMoveToParent()
        self.didMoveToContext()
    }

    internal func willAddChild(_ child: Layout) {
        
        
    }
    internal func didAddChild(_ child: Layout) {
        
        
    }
    
    internal func willMoveToParent(_ parent: LayoutGroup?) {
        
        
    }
    internal func didMoveToParent() {
        
        
    }
    
    internal func willMoveToContext(_ context: LayoutContext?) {
    }
    internal func didMoveToContext() {
    }
    
    fileprivate var childContext: LayoutContext? {
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
    
//    fileprivate func willMoveToParent(_ newParent: Layout?) {
//        self.isParentEnabledInContext = false
//    }
//    fileprivate func didMoveToParent(_ newParent: Layout?) {
//        if let parent = newParent {
//            self.isParentEnabledInContext = parent.isEnabledInContext
//        } else {
//            self.isParentEnabledInContext = false
//        }
//    }
//    fileprivate func didMoveToContext(_ newParent: Layout?) {
//        self.parent = newParent
//        if let parent = newParent {
//            self.isParentEnabledInContext = parent.isEnabledInContext
//        } else {
//            self.isParentEnabledInContext = false
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
    public internal(set) var origin: LayoutVector {
        didSet(oldValue) {
            let newValue = self.origin
            if oldValue != newValue {
                self.resetViewFrame()
                if let parent = self.parent {
                    parent.setNeedsLayoutChilds()
                }
            }
        }
    }
//    public internal(set) var originInContext: LayoutVector?  {
//        didSet(oldValue) {
//            let newValue = self.originInContext
//            if oldValue != newValue {
//                self.didOriginInContextChanged(from: oldValue, to: newValue)
//                self.layoutChildsOriginInContext(newValue)
//                self.resetViewFrame()
//            }
//        }
//    }
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
    
    private static let disabledByParentFlag: UInt32 = 0x1
    private static let disabledFlag: UInt32 = 0x1 << 31
//    private static let movingFlag: UInt32 = 0x1 << 31
//
//    fileprivate(set) var isMoving: Bool {
//        get {
//            return (self.disableFlag & Layout.movingFlag == Layout.movingFlag)
//        }
//        set(newValue) {
//            let oldValue = self.isMoving
//            if oldValue != newValue {
//                if newValue {
//                    self.disableFlag = self.disableFlag | Layout.movingFlag
//                } else {
//                    self.disableFlag = (self.disableFlag & (~Layout.movingFlag))
//                }
//            }
//        }
//    }
    
    private var disableFlag: UInt32 = Layout.disabledByParentFlag {
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
    
    fileprivate(set) var isDisabledByParent: Bool {
        get {
            return (self.disableFlag & Layout.disabledByParentFlag == Layout.disabledByParentFlag)
        }
        set(newValue) {
            let oldValue = self.isDisabledByParent
            if oldValue != newValue {
                if newValue {
                    self.disableFlag = (self.disableFlag & (~Layout.disabledByParentFlag))
                } else {
                    self.disableFlag = self.disableFlag | Layout.disabledByParentFlag
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
            return (self.disableFlag & Layout.disabledFlag == Layout.disabledFlag)
        }
        set(newValue) {
            let oldValue = self.isEnabled
            if oldValue != newValue {
                if newValue {
                    self.disableFlag = (self.disableFlag & (~Layout.disabledFlag))
                } else {
                    self.disableFlag = self.disableFlag | Layout.disabledFlag
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

    internal init(origin: LayoutVector, size: LayoutVector) {
        self.origin = origin
        self.size = size
        self.context = nil
    }

    
    internal func clearOriginInContext() {
        print("clearOriginInContext")
//        self.originInContext = nil
    }
    internal func layoutOriginInContext(parentOriginInContext: LayoutVector) {
        print("layoutOriginInContext")
//        self.originInContext = parentOriginInContext + self.origin
    }
    
    //self changed
    internal func didParentChanged(to: Layout?) {
        print("didIsEnabledChanged")
    }
    internal func isEnabledInContextWillChanged(to: Bool) {
        print("isEnabledInContextWillChanged")
    }
    internal func isEnabledInContextDidChanged(to: Bool) {
        print("isEnabledInContextDidChanged")
    }
    internal func didOriginInContextChanged(from: LayoutVector?, to: LayoutVector?) {
        print("didOriginInContextChanged")
    }
    internal func didOriginChanged(from: LayoutVector, to: LayoutVector) {
        print("didOriginChanged")
    }
    internal func didSizeChanged(from: LayoutVector, to: LayoutVector) {
        print("didOriginChanged")
        self.resetViewFrame()
    }
    
    //child changed

    internal func didChildIsEnabledChanged(_ child: Layout, from: Bool, to: Bool) {
        print("didChildIsEnabledChanged")
    }
    internal func didChildOriginChanged(_ child: Layout, from: LayoutVector, to: LayoutVector) {
        print("didChildOriginChanged")
    }
    internal func didChildSizeChanged(_ child: Layout, from: LayoutVector, to: LayoutVector) {
        print("didChildOriginChanged")
    }
    
    internal func willMove(to context: LayoutContext?) {
        print("willMove")
    }
    
//    internal func didMoveToContext() {
//
//    }
    
    //子类实现
    
    internal func clearChildsOriginInContext() {
        print("clearChildsOriginInContext")
    }
    internal func layoutChildsOriginInContext(_ originInContext: LayoutVector?) {
        print("layoutChildsOriginInContext")
    }
}

public class LayoutGroup: Layout {
    
    public override var view: UIView? {
        willSet(newValue) {
            let oldValue = self.view
            if oldValue != newValue {
                if newValue == nil {
                    self.childContext?.entity.view = nil
                }
            }
        }
        didSet(oldValue) {
            let newValue = self.view
            if oldValue != newValue {
                if newValue != nil {
                    self.childContext?.entity.view = newValue
                }
            }
        }
    }
    
    
    public var isContentContextEnabled: Bool {
        get {
            return self.contentContextEntity != nil
        }
        set(newValue) {
            let oldValue = self.isContentContextEnabled
            if newValue != oldValue {
                if !newValue {
                    self.contentContextEntity = nil
                } else {
                    let entity = LayoutContextEntity()
                    entity.view = self.view
                    entity.viewDidChange = {(context, from, to) -> Void in
                        
                        
                    }
                    entity.viewWillChange = {(context, from, to) -> Void in
                        
                        
                    }
                    self.contentContextEntity = entity
                }
            }
        }
    }
    
    
    public private(set) var contentContextEntity: LayoutContextEntity? = nil {
        willSet(newValue) {
            
        }
        didSet(oldValue) {
            let newValue = self.contentContextEntity
            if newValue != oldValue {
                if newValue == nil {
                    self.resetChildDisabled(to: !self.isEnabledInContext)
                } else {
                    self.resetChildDisabled(to: false)
                }
            }
        }
    }
    internal override func isEnabledInContextWillChanged(to: Bool) {
        if !to && self.contentContextEntity == nil {
            self.resetChildDisabled(to: false)
        }
        print("isEnabledInContextWillChanged")
    }
    internal override func isEnabledInContextDidChanged(to: Bool) {
        print("isEnabledInContextDidChanged")
        if to && self.contentContextEntity == nil {
            self.resetChildDisabled(to: true)
        }
    }
    
    private func resetChildDisabled(to: Bool) {
        for child in self.childs {
            child.isDisabledByParent = to
        }
    }
    
    internal func willRemoveChild(_ child: Layout) {
        
        
    }
    internal func didRemoveChild(_ child: Layout) {
        
        
    }
    
//    private enum ChildContextStyle {
//        case parentContext(context: LayoutContext)
//        case parentContentContext(context: LayoutContext)
//        case none
//    }
    
    private func makeChildContext(context: LayoutContext?, contentContextEntity: LayoutContextEntity?) -> LayoutContext? {
        if let entity = contentContextEntity {
            return LayoutContext.init(entity: entity, origin: LayoutVector())
        } else {
            return context
        }
    }
    
    
    public override var context: LayoutContext? {
        willSet(newValue) {
            let oldValue = self.context
            if oldValue != newValue {
                let oldChildContext = self.makeChildContext(context: oldValue,   contentContextEntity:self.contentContextEntity)
                let newChildContext = self.makeChildContext(context: newValue,   contentContextEntity:self.contentContextEntity)
                if oldChildContext != newChildContext {
                    if oldChildContext != nil {
                        for child in self.childs {
                            child.context = nil
                        }
                    }
                }
            }
        }
        didSet(oldValue) {
            let newValue = self.context
            if oldValue != newValue {
                let oldChildContext = self.makeChildContext(context: oldValue,   contentContextEntity:self.contentContextEntity)
                let newChildContext = self.makeChildContext(context: newValue,   contentContextEntity:self.contentContextEntity)

                if oldChildContext != newChildContext {
                    for child in self.childs {
                        child.context = newChildContext
                    }
//                    let style: ChildContextStyle
//                    if let v = self.contentContext {
//                        style = .parentContentContext(context: v)
//                    } else {
//                        if let v = self.context {
//                            style = .parentContext(context: v)
//                        } else {
//                            style = .none
//                        }
//                    }
//                    switch style {
//                    case .parentContext(let ct):
//                        for child in self.childs {
//                            child.context = ct
//                        }
//                        break
//                    case .parentContentContext(let ct):
//                        var aContext = ct
//                        aContext.origin = LayoutVector(x: 0, y: 0)
//                        for child in self.childs {
//                            child.context = ct
//                        }
//                        break
//                    case .none:
//
//                        break
//                    }
                }
            }
        }
    }
        
        
    fileprivate override var childContext: LayoutContext? {
        return self.makeChildContext(context: self.context,   contentContextEntity:self.contentContextEntity)
    }

    internal private(set) var childs: [Layout] = []
    
    fileprivate func _toStore(child: Layout) {
        self.childs.append(child)
    }
    fileprivate func _clearStore(child: Layout) {
        if let index = self.childs.firstIndex(of: child) {
            self.childs.remove(at: index)
            if let _ = self.childs.firstIndex(of: child) {
                print("error _clearStore \(child)")
            }
        }
    }
    fileprivate func _addChild(child: Layout) {
        child._removeFromParent()
        
        let childContext = self.childContext
        if childContext != nil {
            child.willMoveToContext(childContext)
        }
        self.willAddChild(child)
        child.willMoveToParent(self)
        self._toStore(child: child)
        child.parent = self
        child.context = childContext
        if self.contentContextEntity != nil {
            child.isDisabledByParent = false
        } else {
            child.isDisabledByParent = !self.isEnabledInContext
        }
        child.didMoveToParent()
        self.didAddChild(child)
        if childContext != nil {
            child.didMoveToContext()
        }
    }
    
    public override init(origin: LayoutVector, size: LayoutVector) {
        super.init(origin: origin, size: size)
        self.isEnabled = false
        self.isEnabled = false

    }
    
    public func setNeedsLayoutChilds() {
        
    }
    public func layoutChilds() {
        
        
        
    }
}

public class LinearLayout: Layout {

    
    
    
    
    
    
}
