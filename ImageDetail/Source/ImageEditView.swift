//
//  ImageEditView.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


public struct UIUtil {
    public static func add(_ lhs: UIEdgeInsets, _ rhs: UIEdgeInsets) -> UIEdgeInsets {
        return UIEdgeInsets(top: lhs.top + rhs.top, left: lhs.left + rhs.left, bottom: lhs.bottom + rhs.bottom, right: lhs.right + rhs.right)
    }
    public static func subtract(_ lhs: UIEdgeInsets, _ rhs: UIEdgeInsets) -> UIEdgeInsets {
        return UIEdgeInsets(top: lhs.top - rhs.top, left: lhs.left - rhs.left, bottom: lhs.bottom - rhs.bottom, right: lhs.right - rhs.right)
    }
    
    public static func add(_ lhs: CGPoint, _ rhs: CGPoint) -> CGPoint {
        return CGPoint(x: lhs.x + rhs.x, y: lhs.y + rhs.y)
    }
    public static func subtract(_ lhs: CGPoint, _ rhs: CGPoint) -> CGPoint {
        return CGPoint(x: lhs.x - rhs.x, y: lhs.y - rhs.y)
    }
    
    public static func add(_ lhs: CGSize, _ rhs: CGSize) -> CGSize {
        return CGSize(width: lhs.width + rhs.width, height: lhs.height + rhs.height)
    }
    public static func subtract(_ lhs: CGSize, _ rhs: CGSize) -> CGSize {
        return CGSize(width: lhs.width - rhs.width, height: lhs.height - rhs.height)
    }
    
}



public protocol OrientationContent: class {
    var orientationContentView: UIView {get}
    
    /*
     这个回调中 需要做4件事情，
     1.保存orientationInsets； 2：设置bounds的size大小； 3： 设置center； 4：layoutSubview
     if self.orientationInsets != safeAreaInsets {
     self.orientationInsets = safeAreaInsets
     }
     var bounds = self.bounds
     bounds.size = contentSize
     self.bounds = bounds
     self.center = center
     */
    func updateLayout(_ layout: OrientationView.ContentLayout, option: [AnyHashable:Any])

    
    func willMoveTo(orientationView: OrientationView?)
    func didMoveTo(orientationView: OrientationView?)
}

public protocol OrientationViewDelegate: class {
    func safeAreaInsetsDidChange(of orientationView: OrientationView)

}


open class OrientationView: UIView {
    public enum Direction: Int {
        case anticlockwise = -1
        case clockwise = 1
    }
    
    public private(set) var content: OrientationContent? {
        willSet(newValue) {
            let oldValue = self.content
            if oldValue === newValue {
                return
            }
            if let value = oldValue {
                if value.orientationContentView.superview == self {
                    value.willMoveTo(orientationView: nil)
                    value.orientationContentView.removeFromSuperview()
                    value.didMoveTo(orientationView: nil)
                }
            }
            if let value = newValue {
                if value.orientationContentView.superview != nil {
                    value.willMoveTo(orientationView: nil)
                    value.orientationContentView.removeFromSuperview()
                    value.didMoveTo(orientationView: nil)
                }
            }
        }
        didSet(oldValue) {
            if self.content === oldValue {
                return
            }
            guard let newValue = self.content else {
                return
            }
            newValue.willMoveTo(orientationView: self)
            self.addSubview(newValue.orientationContentView)
            newValue.didMoveTo(orientationView: self)
        }
    }
    
    open func updateContent(_ content: OrientationContent?, option: [AnyHashable:Any] = [:]) {
        self.content = content
        if let value = content {
            self.willLayoutContent(value, layout: self.contentLayout, option:option)
            value.updateLayout(self.contentLayout, option: option)
            self.didLayoutContent(value, layout: self.contentLayout, option:option)
        }
    }

    public struct ContentLayout {
        public var contentInsets: UIEdgeInsets = UIEdgeInsets()
        public var orientation: Int = 0
        public var contentSize: CGSize {
            let size = self.bounds.size
            if self.orientation % 2 != 0 {
                return CGSize(width: size.height, height: size.width)
            } else {
                return size
            }
        }
        fileprivate var bounds: CGRect = CGRect()
        public var contentTransform: CGAffineTransform {
            return CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(self.orientation)))
        }
        public var contentCenter: CGPoint {
            var center = self.bounds.origin
            center.x += (bounds.size.width) / 2
            center.y += (bounds.size.height) / 2
            return center
        }
        
        public mutating func rotate(direction: Direction) {
            self.orientation += direction.rawValue
        }
    }
    

    public private(set) var contentLayout: ContentLayout
    
    
    public func updateFrame(frame: CGRect, contentUpdater: (_ contentLayout: inout ContentLayout) -> [AnyHashable:Any]) {
        var layout = self.contentLayout
        let currentSize = self.frame.size
        var bounds = self.bounds
        bounds.size.width += frame.size.width - currentSize.width
        bounds.size.height += frame.size.height - currentSize.height
        layout.bounds = bounds
        let option = contentUpdater(&layout)
        if let content = self.content {
            self.willLayoutContent(content, layout: layout, option:option)
            self.contentLayout = layout
            self.frame = frame
            content.updateLayout(layout, option: option)
            self.didLayoutContent(content, layout: layout, option:option)
        } else {
            self.contentLayout = layout
            self.frame = frame
        }
    }
    public func updateFrame(frame: CGRect, duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil, contentUpdater: (_ contentLayout: inout ContentLayout) -> [AnyHashable:Any]) {
        var layout = self.contentLayout
        let currentSize = self.frame.size
        var bounds = self.bounds
        bounds.size.width += frame.size.width - currentSize.width
        bounds.size.height += frame.size.height - currentSize.height
        layout.bounds = bounds
        let option = contentUpdater(&layout)
        UIView.animate(withDuration: duration, animations: {
            if let content = self.content {
                self.willLayoutContent(content, layout: layout, option:option)
                self.contentLayout = layout
                self.frame = frame
                content.updateLayout(layout, option: option)
                self.didLayoutContent(content, layout: layout, option:option)
            } else {
                self.contentLayout = layout
                self.frame = frame
            }
        }, completion: completion)
    }
    
    private func updateContentLayout(_ layout: ContentLayout, option: [AnyHashable:Any]) {
        if let content = self.content {
            self.willLayoutContent(content, layout: layout, option:option)
            self.contentLayout = layout
            content.updateLayout(layout, option: option)
            self.didLayoutContent(content, layout: layout, option:option)
        } else {
            self.contentLayout = layout
        }
    }
    
    public func updateContentLayout(updater: (_ contentLayout: inout ContentLayout) -> [AnyHashable:Any]) {
        var layout = self.contentLayout
        let option = updater(&layout)
        self.updateContentLayout(layout, option: option)
    }
    public func updateContentLayout(duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil, updater: (_ contentLayout: inout ContentLayout) -> [AnyHashable:Any]) {
        var layout = self.contentLayout
        let option = updater(&layout)
        UIView.animate(withDuration: duration, animations: {
            self.updateContentLayout(layout, option: option)
        }, completion: completion)
    }
    
    open func willLayoutContent(_ content: OrientationContent, layout: ContentLayout, option: [AnyHashable:Any]) {
        
        
    }
    open func didLayoutContent(_ content: OrientationContent, layout: ContentLayout, option: [AnyHashable:Any]) {
        
        
    }
//    open func layoutContent(_ content: OrientationContent, layout: ContentLayout, option: [AnyHashable:Any]) {
//        content.updateLayout(self.contentLayout, option: option)
//    }
    
    public weak var delegate: OrientationViewDelegate?
    
    public override init(frame: CGRect) {
        self.contentLayout = ContentLayout()
        super.init(frame: frame)
        self.contentLayout.bounds = self.bounds
    }
    required public init?(coder aDecoder: NSCoder) {
        self.contentLayout = ContentLayout()
        super.init(coder: aDecoder)
        self.contentLayout.bounds = self.bounds
    }
    
    open override func layoutSubviews() {
        if self.bounds != self.contentLayout.bounds {
            var layout = self.contentLayout
            layout.bounds = self.bounds
            if let content = self.content {
                self.willLayoutContent(content, layout: layout, option:[:])
                self.contentLayout = layout
                content.updateLayout(layout, option: [:])
                self.didLayoutContent(content, layout: layout, option:[:])
            } else {
                self.contentLayout = layout
            }
        }
        super.layoutSubviews()
        print("{layoutSubviews \(self)}")
    }
    open override var frame: CGRect {
        willSet(newValue) {
            print("willSet frame: current \(self.frame), to:\(newValue)")
        }
        didSet(oldValue) {
            print("didSet frame: current \(self.frame), from:\(oldValue)")
        }
    }
    @available(iOS 11.0, *)
    open override func safeAreaInsetsDidChange() {
        super.safeAreaInsetsDidChange()
        if let delegate = self.delegate {
            delegate.safeAreaInsetsDidChange(of: self)
        }
        print("safeAreaInsetsDidChange \(self) safeAreaInsets:\(self.safeAreaInsets)")
    }
}

internal class ZoomScrollController: NSObject, OrientationContent {
    //        open var safeContentInset: UIEdgeInsets { get }
    internal class _ZoomScrollView: UIScrollView {
        var orientationInsets: UIEdgeInsets = UIEdgeInsets()
        fileprivate private(set) var paddingInset: UIEdgeInsets = UIEdgeInsets()
        fileprivate var onZoomScaleChanged: ((_ view: _ZoomScrollView, _ oldValue: CGFloat) -> Void)?
        
        public override init(frame: CGRect) {
            super.init(frame: frame)
            if #available(iOS 11.0, *) {
                self.contentInsetAdjustmentBehavior = .never
            }
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
            if #available(iOS 11.0, *) {
                self.contentInsetAdjustmentBehavior = .never
            }
        }
    }
    fileprivate class _ZoomViewHandler: NSObject, UIScrollViewDelegate {
        public weak var scrollView: UIScrollView?
        public weak var viewForZooming: UIView?
        
        public func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
            guard scrollView == self.scrollView else {
                return
            }
        }
        public func scrollViewDidEndScrollingAnimation(_ scrollView: UIScrollView) {
            guard scrollView == self.scrollView else {
                return
            }
        }
        public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
            guard scrollView == self.scrollView else {
                return nil
            }
            return self.viewForZooming
        }
        public func scrollViewWillBeginZooming(_ scrollView: UIScrollView, with view: UIView?) {
            guard scrollView == self.scrollView else {
                return
            }
        }
        public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
            guard scrollView == self.scrollView else {
                return
            }
        }
    }
    internal class ZoomScrollLayout: NSObject {
        var contentLayout: OrientationView.ContentLayout = OrientationView.ContentLayout()
        var paddingInset: UIEdgeInsets = UIEdgeInsets()
        var contentInset: UIEdgeInsets = UIEdgeInsets()
        var contentSize: CGSize = CGSize()
        var contentOffset: CGPoint = CGPoint()

        var anchorPoint: CGPoint = CGPoint()

        
        
    }
    
    fileprivate class _ZoomContentView: UIView {
        public override init(frame: CGRect) {
            super.init(frame: frame)
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
        }
    }

    private let _scrollView: _ZoomScrollView
    fileprivate let _contentView: _ZoomContentView
    private let _layout: ZoomScrollLayout

    public var contentView: UIView {
        return self._contentView
    }
    
    public var originalContentSize: CGSize = CGSize() {
        didSet(oldValue) {
            let newValue = self.originalContentSize
            if oldValue != newValue {
                self._contentView.frame = CGRect(origin: CGPoint(), size: newValue)
                self._scrollView.contentSize = newValue
            }
        }
    }
    private let _scrollViewHandler: _ZoomViewHandler
    
    public override init() {
        self._scrollView = _ZoomScrollView()
        self._contentView = _ZoomContentView()
        self._scrollViewHandler = _ZoomViewHandler()
        self._layout = ZoomScrollLayout()
        super.init()
        self._scrollView.addSubview(self._contentView)
        self._scrollViewHandler.scrollView = self._scrollView
        self._scrollViewHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollViewHandler
    }

    open func updateContentSize(_ contentSize: CGSize) {
        self._layout.contentSize = contentSize
        self._scrollView.contentSize = contentSize
    }
    
    
    var orientationContentView: UIView {
        return self._scrollView
    }
    
    
    func layout(_ layout: ZoomScrollLayout) {
        let scrollView = self._scrollView
        scrollView.bounds.size = layout.contentLayout.contentSize
        scrollView.center = layout.contentLayout.contentCenter
        scrollView.transform = layout.contentLayout.contentTransform
        scrollView.contentSize = layout.contentSize
        scrollView.contentInset = layout.contentInset
        scrollView.contentOffset = layout.contentOffset
    }
    
    
    func updateLayout(_ contentLayout: OrientationView.ContentLayout, option: [AnyHashable : Any]) {
        let layout = self._layout
        layout.contentLayout = contentLayout
        
        let size = layout.contentLayout.contentSize
        let safeInset = layout.contentLayout.contentInsets
        var paddingX: CGFloat = size.width - safeInset.left - safeInset.right - layout.contentSize.width
        var paddingY: CGFloat = size.height - safeInset.top - safeInset.bottom - layout.contentSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        layout.paddingInset = paddingInset
        let old = layout.contentInset
        let new = UIUtil.add(safeInset, paddingInset)
        layout.contentInset = new
        layout.contentOffset.x += old.left - new.left
        layout.contentOffset.y += old.top - new.top
        
        self.layout(layout)
    }
    
    func willMoveTo(orientationView: OrientationView?) {
        if let parent = orientationView {
            let layout = self._layout
            layout.contentLayout = parent.contentLayout
            
            let size = layout.contentLayout.contentSize
            
            let safeInset = layout.contentLayout.contentInsets
            var paddingX: CGFloat = size.width - safeInset.left - safeInset.right - layout.contentSize.width
            var paddingY: CGFloat = size.height - safeInset.top - safeInset.bottom - layout.contentSize.height
            if paddingX < 0 {
                paddingX = 0
            }
            if paddingY < 0 {
                paddingY = 0
            }
            let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
            layout.paddingInset = paddingInset
            layout.contentInset = UIUtil.add(safeInset, paddingInset)
            layout.contentOffset.x = layout.contentInset.left * -1
            layout.contentOffset.y = layout.contentInset.top * -1
            self.layout(layout)
        }
    }
    func didMoveTo(orientationView: OrientationView?) {}
    

    
    
    
    fileprivate var onZoomScaleChanged: ((_ view: _ZoomScrollView, _ oldValue: CGFloat) -> Void)?
    
}

