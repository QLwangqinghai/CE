//
//  ImageEditView.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public protocol OrientationContent: class {
    var orientationContentView: UIView {get}
    var orientationInsets: UIEdgeInsets { get }
    
    
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

open class OrientationView: UIView {
    public enum Direction: Int {
        case anticlockwise = -1
        case clockwise = 1
    }
    
    public var content: OrientationContent? {
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
            self.willLayoutContent(newValue)
            self.layoutContent(newValue)
            self.didLayoutContent(newValue)
        }
    }
    
    public struct ContentLayout {
        public var contentInsets: UIEdgeInsets = UIEdgeInsets()
        public var orientation: Int = 0
        public var contentSize: CGSize = CGSize()
        public var contentTransform: CGAffineTransform {
            return CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(self.orientation)))
        }
        public var contentSafeAreaInsets: UIEdgeInsets = UIEdgeInsets()
        public var contentCenter: CGPoint = CGPoint()
    }
    

    public private(set) var contentLayout: ContentLayout = ContentLayout()
//    public var contentCenter: CGPoint {
//        var center = self.bounds.origin
//        center.x += (bounds.size.width) / 2
//        center.y += (bounds.size.height) / 2
//        return center
//    }

    
    private func _prepare() {
        var layout = self.contentLayout
        let bounds = self.bounds
        var contentSize = CGSize()
        if layout.orientation % 2 != 0 {
            contentSize.width = bounds.size.height
            contentSize.height = bounds.size.width
        } else {
            contentSize.width = bounds.size.width
            contentSize.height = bounds.size.height
        }
        var safeAreaInsets = layout.contentInsets
        let array: [CGFloat] = [safeAreaInsets.top, safeAreaInsets.left, safeAreaInsets.bottom, safeAreaInsets.right]
        var idx = (layout.orientation * -1) % 4
        if idx < 0 {
            idx = (idx + 4) % 4
        }
        safeAreaInsets.top = array[idx % 4]
        safeAreaInsets.left = array[(idx + 1) % 4]
        safeAreaInsets.bottom = array[(idx + 2) % 4]
        safeAreaInsets.right = array[(idx + 3) % 4]
        
        var center = bounds.origin
        center.x += (bounds.size.width) / 2
        center.y += (bounds.size.height) / 2
        
        layout.contentCenter = center
        layout.contentSize = contentSize
        layout.contentSafeAreaInsets = safeAreaInsets
    }
    
    open func willLayoutContent(_ content: OrientationContent, option: [AnyHashable:Any]) {
        
        
    }
    open func didLayoutContent(_ content: OrientationContent, option: [AnyHashable:Any]) {
        
        
    }
    open func layoutContent(_ content: OrientationContent, option: [AnyHashable:Any]) {
        let value = self.orientation
        let contentView: UIView = content.orientationContentView
        
        content.updateLayout(self.contentLayout, option: option)

//        content.layout(orientation: self.orientation, orientationInsets: self.contentSafeAreaInsets, center: self.contentCenter, contentSize: self.contentSize)
    }
    
    public override init(frame: CGRect) {
        super.init(frame: frame)
    }
    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    open override func layoutSubviews() {
        if let content = self.content {
            self.willLayoutContent(content)
            self._prepare()
            self.layoutContent(content)
            self.didLayoutContent(content)
        } else {
            self._prepare()
        }
        super.layoutSubviews()
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
        if let content = self.content {
            self.willLayoutContent(content)
            self._prepare()
            self.layoutContent(content)
            self.didLayoutContent(content)
        } else {
            self._prepare()
        }
        print("\(self) safeAreaInsets:\(self.safeAreaInsets)")
    }
    
    open func rotate(direction: Direction, option: [AnyHashable:Any]) {
        self.updateOrientation(self.orientation + direction.rawValue, option: option)
    }
    
    open func rotate(direction: Direction, option: [AnyHashable:Any], duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil) {
        self.updateOrientation(self.orientation + direction.rawValue, option: option, duration:duration, completion:completion)
    }

    open func updateOrientation(_ orientation: Int, option: [AnyHashable:Any]) {
        if self.orientation != orientation {
            self.orientation = orientation
            if let content = self.content {
                self.willLayoutContent(content)
                self._prepare()
                self.layoutContent(content)
                self.didLayoutContent(content)
            } else {
                self._prepare()
            }
        }
    }
    open func updateOrientation(_ orientation: Int, option: [AnyHashable:Any], duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil) {
        UIView.animate(withDuration: duration, animations: {
            self.updateOrientation(orientation, option:option)
        }, completion: completion)
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
    
    
    public private(set) var contentSize: CGSize

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
        self.contentSize = CGSize()
        super.init()
        self._scrollView.addSubview(self._contentView)
        self._scrollViewHandler.scrollView = self._scrollView
        self._scrollViewHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollViewHandler
    }

    open func updateContentSize(_ contentSize: CGSize) {
        self.contentSize = contentSize
        self._scrollView.contentSize = contentSize
    }
    
    
    var orientationContentView: UIView {
        return self._scrollView
    }
    
    func updateLayout(_ layout: OrientationView.ContentLayout, option: [AnyHashable : Any]) {
        
    }
    
    func layout(orientation: Int, orientationInsets: UIEdgeInsets, center: CGPoint, contentSize: CGSize) {
        self.orientationInsets = orientationInsets
        let scrollView = self._scrollView
        var bounds = scrollView.bounds
        bounds.size = contentSize
        scrollView.bounds = bounds
        scrollView.center = center
        
        let safeInset = self.orientationInsets
        let contentSize = scrollView.contentSize
        var paddingX: CGFloat = bounds.size.width - safeInset.left - safeInset.right - contentSize.width
        var paddingY: CGFloat = bounds.size.height - safeInset.top - safeInset.bottom - contentSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        self.paddingInset = paddingInset
        let newContentInset = UIEdgeInsets(top: safeInset.top + paddingInset.top, left: safeInset.left + paddingInset.left, bottom: safeInset.bottom + paddingInset.bottom, right: safeInset.right + paddingInset.right)
        let old = scrollView.contentInset
        if old != newContentInset {
            scrollView.contentInset = newContentInset
//            self.scrollView.contentOffset.x += old.left - newContentInset.left
//            self.scrollView.contentOffset.y += old.top - newContentInset.top
            
        }

    }
    func willMoveTo(orientationView: OrientationView?) {
        if let parent = orientationView {
            let layout = parent.contentLayout
//            self.layout(orientation: parent.orientation, orientationInsets: parent.contentSafeAreaInsets, center: parent.contentCenter, contentSize: parent.contentSize)
            let scrollView = self._scrollView
            let safeInset = self.orientationInsets
            let contentSize = scrollView.contentSize

            scrollView.transform = layout.contentTransform
            var bounds = scrollView.bounds
            bounds.size = layout.contentSize
            scrollView.bounds = bounds
            scrollView.center = layout.contentCenter
            var paddingX: CGFloat = bounds.size.width - safeInset.left - safeInset.right - contentSize.width
            var paddingY: CGFloat = bounds.size.height - safeInset.top - safeInset.bottom - contentSize.height
            if paddingX < 0 {
                paddingX = 0
            }
            if paddingY < 0 {
                paddingY = 0
            }
            
            let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
            self.paddingInset = paddingInset
            let newContentInset = UIEdgeInsets(top: safeInset.top + paddingInset.top, left: safeInset.left + paddingInset.left, bottom: safeInset.bottom + paddingInset.bottom, right: safeInset.right + paddingInset.right)
            scrollView.contentInset = newContentInset
            scrollView.contentOffset.x = newContentInset.left * -1
            scrollView.contentOffset.y = newContentInset.top * -1
        }
    }
    func didMoveTo(orientationView: OrientationView?) {}
    
    private(set) var orientationInsets: UIEdgeInsets = UIEdgeInsets()
    fileprivate private(set) var paddingInset: UIEdgeInsets = UIEdgeInsets()
    fileprivate private(set) var anchorPoint: CGPoint = CGPoint()

    
    
    
    fileprivate var onZoomScaleChanged: ((_ view: _ZoomScrollView, _ oldValue: CGFloat) -> Void)?
    
}

