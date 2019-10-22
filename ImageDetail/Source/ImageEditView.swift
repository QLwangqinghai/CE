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

open class ZoomView: UIView {
    fileprivate class _ZoomHandler: NSObject, UIScrollViewDelegate {
        public weak var scrollView: UIScrollView?
        public weak var viewForZooming: UIView?
        public var didZoom: ((_ handler: _ZoomHandler, _ scrollView: UIScrollView) -> Void)?
//        public func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
//            guard scrollView == self.scrollView else {
//                return
//            }
//        }
//        public func scrollViewDidEndScrollingAnimation(_ scrollView: UIScrollView) {
//            guard scrollView == self.scrollView else {
//                return
//            }
//        }
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
            guard let aview = view else {
                return
            }
            print("scrollViewWillBeginZooming \(scrollView) with:\(aview)")
        }
        public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
            guard scrollView == self.scrollView else {
                return
            }
            guard let aview = view else {
                return
            }
            print("scrollViewDidEndZooming \(scrollView) at:\(aview) scale:\(scale)")
        }
        public func scrollViewDidZoom(_ scrollView: UIScrollView) {
            guard scrollView == self.scrollView else {
                return
            }
            if let didZoom = self.didZoom {
                didZoom(self, scrollView)
            }
        }
    }
    
    private let _scrollView: UIScrollView
    private let _scrollZoomHandler: _ZoomHandler
    public let contentView: UIView

    private func _resetContentInset() {
        let scrollView = self._scrollView
        let old = scrollView.contentInset
        let new = UIUtil.add(self.orientationInset, self.paddingInset)
        scrollView.contentInset = new
//        var offset = scrollView.contentOffset
//        offset.x = offset.x - (new.left - old.left)
//        offset.y = offset.y - (new.top - old.top)
//        scrollView.contentOffset = offset
    }
    
    public var orientationInset: UIEdgeInsets = UIEdgeInsets() {
        didSet {
            self._resetContentInset()
        }
    }
    public fileprivate(set) var paddingInset: UIEdgeInsets = UIEdgeInsets() {
        didSet {
            self._resetContentInset()
        }
    }
    
    private func _resetPadding(scrollView: UIScrollView) {
        let zoomSize = scrollView.contentSize
        let scrollSize = scrollView.bounds.size
        let safeInset = self.orientationInset
        var paddingH: CGFloat = scrollSize.width - safeInset.left - safeInset.right - zoomSize.width
        var paddingV: CGFloat = scrollSize.height - safeInset.top - safeInset.bottom - zoomSize.height
        if paddingH < 0 {
            paddingH = 0
        } else {
            paddingH = paddingH / 2
        }
        if paddingV < 0 {
            paddingV = 0
        } else {
            paddingV = paddingV / 2
        }
        let paddingInset = UIEdgeInsets(top: paddingV, left: paddingH, bottom: paddingV, right: paddingH)
        self.paddingInset = paddingInset
    }

    private func _prepareBase() {
        let scrollView = self._scrollView
        var scrollViewFrame = self.bounds
        scrollViewFrame.origin = CGPoint()
        if #available(iOS 11.0, *) {
            scrollView.contentInsetAdjustmentBehavior = .never
        }
        scrollView.frame = scrollViewFrame
        scrollView.addSubview(self.contentView)
        self.addSubview(scrollView)
        self._resetPadding(scrollView: scrollView)
        self._scrollZoomHandler.didZoom = {[weak self](handler, scrollView) in
            guard let `self` = self else {
                return
            }
            self._resetPadding(scrollView: scrollView)
        }
        
        self._scrollZoomHandler.scrollView = scrollView
        self._scrollZoomHandler.viewForZooming = self.contentView
        self._scrollView.delegate = self._scrollZoomHandler
    }
    
    public override init(frame: CGRect) {
        self._scrollView = UIScrollView()
        self._scrollZoomHandler = _ZoomHandler()
        self.contentView = UIView()
        super.init(frame: frame)
        self._prepareBase()
    }
    required public init?(coder aDecoder: NSCoder) {
        self._scrollView = UIScrollView()
        self._scrollZoomHandler = _ZoomHandler()
        self.contentView = UIView()
        super.init(coder: aDecoder)
        self._prepareBase()
    }
    
    public func resetContent(size: CGSize) {
        let scrollView = self._scrollView
        let scrollSize = scrollView.bounds.size
        guard scrollSize.width > 0 && scrollSize.height > 0 else {
            self.resetContent(size: size, zoomScale: 1.0, minimumZoomScale: 1.0, maximumZoomScale: 1.0)
            return
        }
        
        let xScale = size.width / scrollSize.width
        let yScale = size.height / scrollSize.height
        var minimumZoomScale: CGFloat = 1.0
        var maximumZoomScale: CGFloat = 1.0
        var zoomScale: CGFloat = 1.0
        if xScale > yScale {
            minimumZoomScale = scrollSize.width / size.width / 4.0
            maximumZoomScale = size.width / scrollSize.width * 4.0
        } else {
            minimumZoomScale = scrollSize.height / size.height / 4.0
            maximumZoomScale = size.height / scrollSize.height * 4.0
        }
        zoomScale = CGFloat(sqrt(Double(minimumZoomScale * maximumZoomScale)))
        self.resetContent(size: size, zoomScale: zoomScale, minimumZoomScale: minimumZoomScale, maximumZoomScale: maximumZoomScale)
    }

    public func resetContent(size: CGSize, zoomScale: CGFloat, minimumZoomScale: CGFloat, maximumZoomScale: CGFloat) {
        self._resetContentInset()
        let scrollView = self._scrollView
        let contentView = self.contentView
        scrollView.minimumZoomScale = minimumZoomScale
        scrollView.maximumZoomScale = maximumZoomScale
        scrollView.zoomScale = zoomScale
        scrollView.contentSize = size
        contentView.bounds.size = size
        contentView.center = CGPoint(x: size.width / 2, y: size.height / 2)
        self._resetPadding(scrollView: scrollView)
    }
}

internal class ZoomScrollController: NSObject, UIGestureRecognizerDelegate {

    internal struct ZoomLayout {
        private(set) var scrollContentInset: UIEdgeInsets = UIEdgeInsets()
        var zoomSize: CGSize = CGSize()

        var contentInset: UIEdgeInsets = UIEdgeInsets() {
            didSet {
                self.resetScrollContentInset()
            }
        }
        var paddingInset: UIEdgeInsets = UIEdgeInsets() {
            didSet {
                self.resetScrollContentInset()
            }
        }
        
        var scrollSize: CGSize = CGSize()
        var originalSize: CGSize = CGSize() {
            didSet {
                self.resetZoomSize()
            }
        }
        
        
        var zoomScale: CGFloat = 1.0  {
            didSet {
                self.resetZoomSize()
            }
        }
        var maximumZoomScale: CGFloat = 1.0
        var minimumZoomScale: CGFloat = 1.0
    
        private mutating func resetZoomSize() {
            var size = self.originalSize
            size.width *= self.zoomScale
            size.height *= self.zoomScale
            self.zoomSize = size
        }
        private mutating func resetScrollContentInset() {
            self.scrollContentInset = UIUtil.add(self.contentInset, self.paddingInset)
        }
    }
    
//    internal struct PinchContext {
//        var contentInset: UIEdgeInsets = UIEdgeInsets()
//        var paddingInset: UIEdgeInsets = UIEdgeInsets()
//
//        var contentSize: CGSize = CGSize()
//        var contentOffset: CGPoint = CGPoint()
//
//        var anchorPoint: CGPoint = CGPoint()
//    }
    internal class ZoomScrollLayout: NSObject {
        var contentLayout: OrientationView.ContentLayout = OrientationView.ContentLayout()
        var zoomLayout: ZoomLayout = ZoomLayout()
    }
    
    fileprivate class _ZoomContentView: UIView {
        public override init(frame: CGRect) {
            super.init(frame: frame)
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
        }
        override func layoutSubviews() {
            super.layoutSubviews()
            print("_ZoomContentView layoutSubviews frame:\(self.frame) bounds: \(self.bounds)")
        }
    }

    public let zoomView: ZoomView
    private let _layout: ZoomScrollLayout

    public var contentView: UIView {
        return self.zoomView.contentView
    }
        
    public override init() {
        self.zoomView = ZoomView()
        self._layout = ZoomScrollLayout()
        super.init()
    }
    
    func goodContentOffset(of scrollView: UIScrollView, contentOffset: CGPoint) -> CGPoint {
        let scrollSize = scrollView.bounds.size
        var offset = contentOffset
        let minX = 0 - scrollView.contentInset.left
        let minY = 0 - scrollView.contentInset.top
        let maxX = scrollView.contentSize.width - scrollSize.width + scrollView.contentInset.right
        let maxY = scrollView.contentSize.height - scrollSize.height + scrollView.contentInset.bottom
        if offset.x < minX {
            offset.x = minX
        } else if offset.x > maxX {
            offset.x = maxX
        }
        if offset.y < minY {
             offset.y = minY
        } else if offset.y > maxY {
             offset.y = maxY
        }
        return offset
    }

    public func updateZoomLayout(updater: (_ zoomLayout: inout ZoomLayout) -> Void) {
        var layout = self._layout.zoomLayout
        updater(&layout)
        self._layout.zoomLayout = layout
        self.zoomLayout(layout)
    }
    public func updateZoomLayout(duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil, updater: (_ zoomLayout: inout ZoomLayout) -> Void) {
        var layout = self._layout.zoomLayout
        updater(&layout)
        self._layout.zoomLayout = layout
        UIView.animate(withDuration: duration, animations: {
            self.zoomLayout(layout)
        }, completion: completion)
    }
    
    func zoomLayout(_ layout: ZoomLayout) {
//        let scrollView = self._scrollView
//        scrollView.contentInset = layout.scrollContentInset
//        var frame = self._contentView.frame
//        frame.size = layout.zoomSize
//        self._contentView.frame = frame
    }
    
    func layout(_ layout: ZoomScrollLayout) {
        let zoomView = self.zoomView
        let contentLayout = layout.contentLayout
        
        zoomView.bounds.size = contentLayout.contentSize
        zoomView.center = contentLayout.contentCenter
        zoomView.transform = contentLayout.contentTransform
        
        var zoomLayout = layout.zoomLayout
        zoomLayout.contentInset = layout.contentLayout.contentInsets
        zoomLayout.scrollSize = layout.contentLayout.contentSize
        let zoomSize = zoomLayout.zoomSize
        let scrollSize = zoomLayout.scrollSize
        let safeInset = zoomLayout.contentInset
        var paddingX: CGFloat = scrollSize.width - safeInset.left - safeInset.right - zoomSize.width
        var paddingY: CGFloat = scrollSize.height - safeInset.top - safeInset.bottom - zoomSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        zoomLayout.paddingInset = paddingInset
        layout.zoomLayout = zoomLayout
        
        zoomView.orientationInset = zoomLayout.scrollContentInset
    }

}


extension ZoomScrollController: OrientationContent {
    var orientationContentView: UIView {
        return self.zoomView
    }
    
    func updateLayout(_ contentLayout: OrientationView.ContentLayout, option: [AnyHashable : Any]) {
        let layout = self._layout
        layout.contentLayout = contentLayout
        self.layout(layout)
    }
    
    func willMoveTo(orientationView: OrientationView?) {
        if let parent = orientationView {
            let layout = self._layout
            layout.contentLayout = parent.contentLayout
            self.layout(layout)
//            self.zoomView.resetContent(size: <#T##CGSize#>)

        }
    }
    func didMoveTo(orientationView: OrientationView?) {}
}


public class ZoomItem: NSObject {
    public var frame: CGRect {
        didSet {
            self.resetFrame()
        }
    }
    public let view: UIView
    public init(view: UIView) {
        self.view = view
        self.frame = view.frame
        super.init()
    }
    private func resetFrame() {
        self.view.frame = frame
    }
}
