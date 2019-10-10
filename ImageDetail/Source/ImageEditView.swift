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

internal class ZoomScrollController: NSObject, UIGestureRecognizerDelegate {
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
//    fileprivate class _ZoomViewHandler: NSObject, UIScrollViewDelegate {
//        public weak var scrollView: UIScrollView?
//        public weak var viewForZooming: UIView?
//
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
//        public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
//            guard scrollView == self.scrollView else {
//                return nil
//            }
//            return self.viewForZooming
//        }
//        public func scrollViewWillBeginZooming(_ scrollView: UIScrollView, with view: UIView?) {
//            guard scrollView == self.scrollView else {
//                return
//            }
//        }
//        public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
//            guard scrollView == self.scrollView else {
//                return
//            }
//            guard let aview = view else {
//                return
//            }
//            print("scrollViewDidEndZooming \(scrollView) at:\(aview) scale:\(scale)")
//        }
//
//        public func scrollViewDidZoom(_ scrollView: UIScrollView) {
//            print("scrollViewDidZoom \(scrollView)")
//        }
//    }
    
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
//    private let _scrollViewHandler: _ZoomViewHandler
    
    private let pinchGestureRecognizer: UIPinchGestureRecognizer
    

    
    public override init() {
        self._scrollView = _ZoomScrollView()
        self._contentView = _ZoomContentView()
//        self._scrollViewHandler = _ZoomViewHandler()
        self._layout = ZoomScrollLayout()
        self.pinchGestureRecognizer = UIPinchGestureRecognizer()
        super.init()
        self._scrollView.addSubview(self._contentView)
//        self._scrollViewHandler.scrollView = self._scrollView
//        self._scrollViewHandler.viewForZooming = self._contentView
//        self._scrollView.delegate = self._scrollViewHandler
        
        self._scrollView.addGestureRecognizer(self.pinchGestureRecognizer)
        self.pinchGestureRecognizer.delegate = self
        self.pinchGestureRecognizer.addTarget(self, action: #selector(handlePinchGestureRecognizer(_:)))
        self._scrollView.panGestureRecognizer.isEnabled = false
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
    
    
    var pinchBeginPointInContent: (CGPoint, CGPoint, CGFloat)?
    @objc func handlePinchGestureRecognizer(_ recognizer: UIPinchGestureRecognizer) {
        guard recognizer.numberOfTouches == 2 else {
            print("error recognizer, numberOfTouches: \(recognizer.numberOfTouches), state: \(recognizer.state) ")

            return
        }

        var zoomSize = self._contentView.bounds.size
        guard zoomSize.width > 0 && zoomSize.height > 0 else {
            return
        }
        var zoomLayout = self._layout.zoomLayout
        let scrollSize = zoomLayout.scrollSize
        
        switch recognizer.state {
        case .possible:
            break
        case .began:
            var center = UIUtil.add(recognizer.location(ofTouch: 0, in: recognizer.view), recognizer.location(ofTouch: 1, in: recognizer.view))
            center.x /= 2
            center.y /= 2
            let point = self._scrollView.convert(center, to: self._contentView)
            
            let anchorPoint = UIUtil.subtract(center, self._scrollView.bounds.origin)
            guard zoomSize.width >= point.x && zoomSize.height >= point.y && point.x >= 0 && point.y >= 0 else {
                self.pinchBeginPointInContent = nil
                return
            }
            self.pinchBeginPointInContent = (CGPoint(x: anchorPoint.x / scrollSize.width, y: anchorPoint.y / scrollSize.height), CGPoint(x: point.x / zoomSize.width, y: point.y / zoomSize.height), zoomLayout.zoomScale)
            break
        case .changed:
            guard let (anchorPointScale, contentAnchorPointScale, scale) = self.pinchBeginPointInContent else {
                return
            }
            zoomLayout.zoomScale = recognizer.scale * scale
            zoomSize = zoomLayout.zoomSize
            self._contentView.frame.size = zoomSize
            self._scrollView.contentSize = zoomSize
            
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
            self._scrollView.contentInset = zoomLayout.scrollContentInset

            let anchorPoint = UIUtil.add(CGPoint(x: anchorPointScale.x * scrollSize.width, y: anchorPointScale.y * scrollSize.height), self._scrollView.bounds.origin)
            let anchorPointInContent = self._scrollView.convert(anchorPoint, to: self._contentView)
            
            let contentAnchorPoint = CGPoint(x: contentAnchorPointScale.x * zoomSize.width, y: contentAnchorPointScale.y * zoomSize.height)
            
            let contentOffsetChanged = UIUtil.subtract(contentAnchorPoint, anchorPointInContent)
            
            var contentOffset = UIUtil.add(self._scrollView.contentOffset, contentOffsetChanged)
            self._scrollView.contentOffset = self.goodContentOffset(of: self._scrollView, contentOffset: contentOffset)
            break
        case .ended:
            fallthrough
        case .cancelled:
            fallthrough
        case .failed:
            fallthrough
        @unknown default:
            guard let (anchorPointScale, contentAnchorPointScale, scale) = self.pinchBeginPointInContent else {
                return
            }
            self.pinchBeginPointInContent = nil
        }
        self._layout.zoomLayout = zoomLayout
        print("state: \(recognizer.state) scale:\(recognizer.scale) l0:\(recognizer.location(ofTouch: 0, in: recognizer.view)) l1:\(recognizer.location(ofTouch: 1, in: recognizer.view))")
    }
    
    func gestureRecognizerShouldBegin(_ gestureRecognizer: UIGestureRecognizer) -> Bool {
        guard gestureRecognizer == self.pinchGestureRecognizer else {
            return true
        }
        guard gestureRecognizer.numberOfTouches == 2 else {
            print("gestureRecognizerShouldBegin error recognizer, numberOfTouches: \(gestureRecognizer.numberOfTouches), state: \(gestureRecognizer.state) ")
            return true
        }
        let size = self._contentView.bounds.size
        guard size.width > 0 && size.height > 0 else {
            return false
        }
        
        var center = UIUtil.add(gestureRecognizer.location(ofTouch: 0, in: gestureRecognizer.view), gestureRecognizer.location(ofTouch: 1, in: gestureRecognizer.view))
        center.x /= 2
        center.y /= 2
        return self.contentView.frame.contains(self._scrollView.convert(center, to: self._contentView))
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
        let scrollView = self._scrollView
        scrollView.contentInset = layout.scrollContentInset
        var frame = self._contentView.frame
        frame.size = layout.zoomSize
        self._contentView.frame = frame
    }
    
    func layout(_ layout: ZoomScrollLayout) {
        let scrollView = self._scrollView
        let contentLayout = layout.contentLayout
        
        scrollView.bounds.size = contentLayout.contentSize
        scrollView.center = contentLayout.contentCenter
        scrollView.transform = contentLayout.contentTransform
        
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
        
        scrollView.contentInset = zoomLayout.scrollContentInset
    }

    fileprivate var onZoomScaleChanged: ((_ view: _ZoomScrollView, _ oldValue: CGFloat) -> Void)?
    
}


extension ZoomScrollController: OrientationContent {
    var orientationContentView: UIView {
        return self._scrollView
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
            
            var contentOffset = self._scrollView.contentOffset
            contentOffset.x = 0 - layout.zoomLayout.scrollContentInset.left
            contentOffset.y = 0 - layout.zoomLayout.scrollContentInset.top
            self._scrollView.contentOffset = contentOffset
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
    public var zoomScale: CGFloat {
       didSet {
           self.resetFrame()
       }
    }
    public init(view: UIView) {
        self.view = view
        self.frame = view.frame
        self.zoomScale = 1.0
        super.init()
    }
    private func resetFrame() {
        var frame = self.frame
        let zoomScale = self.zoomScale
        frame.origin.x *= zoomScale
        frame.origin.y *= zoomScale
        frame.size.width *= zoomScale
        frame.size.height *= zoomScale
        self.view.frame = frame
    }
}
