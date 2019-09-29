//
//  ImageEditView.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

fileprivate protocol _ZoomViewDelegate: class {
    func _zoomViewWillLayoutSubviews()
    func _zoomViewDidLayoutSubviews()
}

public protocol OrientationContent: class {
    var orientationContentView: UIView {get}
    var orientationSafeAreaInsets: UIEdgeInsets { get }
    
    
    /*
     这个回调中 需要做4件事情，
     1.保存orientationSafeAreaInsets； 2：设置bounds的size大小； 3： 设置center； 4：layoutSubview
     if self.orientationSafeAreaInsets != safeAreaInsets {
     self.orientationSafeAreaInsets = safeAreaInsets
     }
     var bounds = self.bounds
     bounds.size = contentSize
     self.bounds = bounds
     self.center = center
     */
    func layout(orientation: Int, orientationSafeAreaInsets: UIEdgeInsets, center: CGPoint, contentSize: CGSize)

    
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
            self.layoutContent()
        }
    }

    public private(set) var orientation: Int = 0
    
    
    open func layoutContent() {
        guard let content = self.content else {
            return
        }
        let value = self.orientation
        let contentView: UIView  = content.orientationContentView
        let bounds = self.bounds
        
        var contentSize = CGSize()
        if value % 2 != 0 {
            contentSize.width = bounds.size.height
            contentSize.height = bounds.size.width
        } else {
            contentSize.width = bounds.size.width
            contentSize.height = bounds.size.height
        }
        var center = self.bounds.origin
        center.x += (bounds.size.width) / 2
        center.y += (bounds.size.height) / 2
        contentView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(value)))
        
        var safeAreaInsets = self.safeAreaInsets
        let array: [CGFloat] = [safeAreaInsets.top, safeAreaInsets.left, safeAreaInsets.bottom, safeAreaInsets.right]
        var idx = (value * -1) % 4
        if idx < 0 {
            idx = (idx + 4) % 4
        }
        safeAreaInsets.top = array[idx % 4]
        safeAreaInsets.left = array[(idx + 1) % 4]
        safeAreaInsets.bottom = array[(idx + 2) % 4]
        safeAreaInsets.right = array[(idx + 3) % 4]
        content.layout(orientation: self.orientation, orientationSafeAreaInsets: safeAreaInsets, center: center, contentSize: contentSize)
    }
    
    public override init(frame: CGRect) {
        super.init(frame: frame)
    }
    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    open override func layoutSubviews() {
        self.layoutContent()
        super.layoutSubviews()
    }
    
    @available(iOS 11.0, *)
    open override func safeAreaInsetsDidChange() {
        super.safeAreaInsetsDidChange()
        self.layoutContent()
        print("\(self) safeAreaInsets:\(self.safeAreaInsets)")
    }
    
    open func rotate(direction: Direction) {
        self.updateOrientation(self.orientation + direction.rawValue)
    }
    
    open func rotate(direction: Direction, duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil) {
        self.updateOrientation(self.orientation + direction.rawValue, duration:duration, completion:completion)
    }

    open func updateOrientation(_ orientation: Int) {
        if self.orientation != orientation {
            self.orientation = orientation
            self.layoutContent()
        }
    }
    open func updateOrientation(_ orientation: Int, duration: CFTimeInterval, completion: ((Bool) -> Void)? = nil) {
        self.orientation = orientation
        UIView.animate(withDuration: duration, animations: {
            self.layoutContent()
        }, completion: completion)
    }
}

internal class ZoomScrollController: NSObject, OrientationContent {
    //        open var safeContentInset: UIEdgeInsets { get }
    
    internal class _ZoomScrollView: UIScrollView {
        var orientationSafeAreaInsets: UIEdgeInsets = UIEdgeInsets()
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
    
    fileprivate class _ZoomContentView: UIView {
        public override init(frame: CGRect) {
            super.init(frame: frame)
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
        }
    }

    private let _scrollView: _ZoomScrollView
    internal var scrollView: UIScrollView {
        return self._scrollView
    }
    fileprivate let _contentView: _ZoomContentView
    
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
    fileprivate weak var _zoomViewDelegate: _ZoomViewDelegate?
    private let _scrollViewHandler: _ZoomViewHandler
    
    public override init() {
        self._scrollView = _ZoomScrollView()
        self._contentView = _ZoomContentView()
        self._scrollViewHandler = _ZoomViewHandler()
        super.init()
        self._scrollView.addSubview(self._contentView)
        self._scrollViewHandler.scrollView = self._scrollView
        self._scrollViewHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollViewHandler
    }
    
    
    
    var orientationContentView: UIView {
        return self._scrollView
    }
    
    func layout(orientation: Int, orientationSafeAreaInsets: UIEdgeInsets, center: CGPoint, contentSize: CGSize) {
        if self.orientationSafeAreaInsets != orientationSafeAreaInsets {
            self.orientationSafeAreaInsets = orientationSafeAreaInsets
        }
        var bounds = self.scrollView.bounds
        bounds.size = contentSize
        self.scrollView.bounds = bounds
        self.scrollView.center = center
        
        let safeAreaInsets = self.orientationSafeAreaInsets
        let contentSize = self.scrollView.contentSize
        var paddingX: CGFloat = bounds.size.width - safeAreaInsets.left - safeAreaInsets.right - contentSize.width
        var paddingY: CGFloat = bounds.size.height - safeAreaInsets.top - safeAreaInsets.bottom - contentSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        self.update(safeInset: safeAreaInsets, paddingInset: paddingInset)
    }
    func willMoveTo(orientationView: OrientationView?) {}
    func didMoveTo(orientationView: OrientationView?) {}
    
    var orientationSafeAreaInsets: UIEdgeInsets = UIEdgeInsets()
    fileprivate private(set) var paddingInset: UIEdgeInsets = UIEdgeInsets()
    
    internal func update(safeInset: UIEdgeInsets, paddingInset: UIEdgeInsets) {
        self.orientationSafeAreaInsets = safeInset
        self.paddingInset = paddingInset
        let newContentInset = UIEdgeInsets(top: safeInset.top + paddingInset.top, left: safeInset.left + paddingInset.left, bottom: safeInset.bottom + paddingInset.bottom, right: safeInset.right + paddingInset.right)
        let old = self.scrollView.contentInset
        if old != newContentInset {
            self.scrollView.contentInset = newContentInset
            self.scrollView.contentOffset.x += ((old.left - newContentInset.left) - (old.right - newContentInset.right)) / 2
            self.scrollView.contentOffset.y += ((old.top - newContentInset.top) - (old.bottom - newContentInset.bottom)) / 2
        }
    }
    
    fileprivate var onZoomScaleChanged: ((_ view: _ZoomScrollView, _ oldValue: CGFloat) -> Void)?
    
}

//open class ZoomView: OrientationView {
//    private let _scrollView: _ZoomScrollView
//    internal var scrollView: UIScrollView {
//        return self._scrollView
//    }
//    fileprivate let _contentView: _ZoomContentView
//
//    public var contentView: UIView {
//        return self._contentView
//    }
//
//    public var originalContentSize: CGSize = CGSize() {
//        didSet(oldValue) {
//            let newValue = self.originalContentSize
//            if oldValue != newValue {
//                self._contentView.frame = CGRect(origin: CGPoint(), size: newValue)
//                self._scrollView.contentSize = newValue
//            }
//        }
//    }
//    fileprivate weak var _zoomViewDelegate: _ZoomViewDelegate?
//
//    public private(set) var rotate90CcwCount: UInt = 0
//    private let _scrollViewHandler: _ZoomViewHandler
//
//    public override init(frame: CGRect) {
//        self._scrollView = _ZoomScrollView(frame: CGRect(origin: CGPoint(), size: frame.size))
//        self._contentView = _ZoomContentView()
//        self._scrollViewHandler = _ZoomViewHandler()
//        super.init(frame: frame)
//        self.addSubview(self._scrollView)
//        self.clipsToBounds = true
//        self._scrollView.addSubview(self._contentView)
//        self._scrollViewHandler.scrollView = self._scrollView
//        self._scrollViewHandler.viewForZooming = self._contentView
//        self._scrollView.delegate = self._scrollViewHandler
//
//        self.content = self._scrollView
//    }
//    required public init?(coder aDecoder: NSCoder) {
//        self._scrollView = _ZoomScrollView()
//        self._contentView = _ZoomContentView()
//        self._scrollViewHandler = _ZoomViewHandler()
//        super.init(coder: aDecoder)
//        self.clipsToBounds = true
//        self.addSubview(self._scrollView)
//        self._scrollView.addSubview(self._contentView)
//        self._scrollViewHandler.scrollView = self._scrollView
//        self._scrollViewHandler.viewForZooming = self._contentView
//        self._scrollView.delegate = self._scrollViewHandler
//
//        self.content = self._scrollView
//    }
//}

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
