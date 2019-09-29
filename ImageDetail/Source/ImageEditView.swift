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
        
//        var contentViewBounds = contentView.bounds
//        let swapXY = value % 2 != 0
//        if swapXY {
//            contentViewBounds.size.width = bounds.size.height
//            contentViewBounds.size.height = bounds.size.width
//        } else {
//            contentViewBounds.size.width = bounds.size.width
//            contentViewBounds.size.height = bounds.size.height
//        }
//        contentView.bounds = contentViewBounds
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

open class ZoomView: OrientationView {
    internal class _ZoomScrollView: UIScrollView, OrientationContent {
//        open var safeContentInset: UIEdgeInsets { get }
        var orientationContentView: UIView {
            return self
        }
        override var contentInset: UIEdgeInsets {
            didSet(oldValue) {
//                print("contentInset changed from:\(oldValue) to:\(self.contentInset)")
            }
        }
        func layout(orientation: Int, orientationSafeAreaInsets: UIEdgeInsets, center: CGPoint, contentSize: CGSize) {
            if self.orientationSafeAreaInsets != orientationSafeAreaInsets {
                self.orientationSafeAreaInsets = orientationSafeAreaInsets
            }
            var bounds = self.bounds
            bounds.size = contentSize
            self.bounds = bounds
            self.center = center
            
            var safeAreaInsets = self.orientationSafeAreaInsets
            let contentSize = self.contentSize
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
        
        var orientationSafeAreaInsets: UIEdgeInsets = UIEdgeInsets() {
            didSet(oldValue) {
                let newValue = self.orientationSafeAreaInsets
                if oldValue != newValue {
                    let change = UIEdgeInsets(top: newValue.top - oldValue.top, left: newValue.left - oldValue.left, bottom: newValue.bottom - oldValue.bottom, right: newValue.right - oldValue.right)
                    var contentInset = self.contentInset
                    contentInset.top += change.top
                    contentInset.left += change.left
                    contentInset.bottom += change.bottom
                    contentInset.right += change.right

                    var contentOffset: CGPoint = self.contentOffset
                    contentOffset.x += change.left
                    contentOffset.y += change.top
                    
                    if contentInset != self.contentInset {
                        self.contentInset = contentInset
                    }
                    if contentOffset != self.contentOffset {
                        self.contentOffset = contentOffset
                    }
                    print("orientationSafeAreaInsets changed from:\(oldValue) to:\(newValue)")
                }
            }
        }
        fileprivate private(set) var paddingInset: UIEdgeInsets = UIEdgeInsets()

        internal func update(safeInset: UIEdgeInsets, paddingInset: UIEdgeInsets) {
            self.orientationSafeAreaInsets = safeInset
            self.paddingInset = paddingInset
            let newContentInset = UIEdgeInsets(top: safeInset.top + paddingInset.top, left: safeInset.left + paddingInset.left, bottom: safeInset.bottom + paddingInset.bottom, right: safeInset.right + paddingInset.right)
            let old = self.contentInset
            if old != newContentInset {
                self.contentInset = newContentInset
                self.contentOffset.x += ((old.left - newContentInset.left) - (old.right - newContentInset.right)) / 2
                self.contentOffset.y += ((old.top - newContentInset.top) - (old.bottom - newContentInset.bottom)) / 2
            }
        }
        
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
    
    public private(set) var rotate90CcwCount: UInt = 0
    private let _scrollViewHandler: _ZoomViewHandler

    public override init(frame: CGRect) {
        self._scrollView = _ZoomScrollView(frame: CGRect(origin: CGPoint(), size: frame.size))
        self._contentView = _ZoomContentView()
        self._scrollViewHandler = _ZoomViewHandler()
        super.init(frame: frame)
        self.addSubview(self._scrollView)
        self.clipsToBounds = true
        self._scrollView.addSubview(self._contentView)
        self._scrollViewHandler.scrollView = self._scrollView
        self._scrollViewHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollViewHandler
        
        self.content = self._scrollView
    }
    required public init?(coder aDecoder: NSCoder) {
        self._scrollView = _ZoomScrollView()
        self._contentView = _ZoomContentView()
        self._scrollViewHandler = _ZoomViewHandler()
        super.init(coder: aDecoder)
        self.clipsToBounds = true
        self.addSubview(self._scrollView)
        self._scrollView.addSubview(self._contentView)
        self._scrollViewHandler.scrollView = self._scrollView
        self._scrollViewHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollViewHandler
        
        self.content = self._scrollView
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

extension ZoomView : UIScrollViewDelegate {

}



//open class ZoomController: NSObject {
//    fileprivate class _ZoomContentView: UIView {
//        public override init(frame: CGRect) {
//            super.init(frame: frame)
//        }
//
//        required public init?(coder aDecoder: NSCoder) {
//            super.init(coder: aDecoder)
//        }
//    }
//
//    public class Content: NSObject {
//        public var zoomScale: CGFloat
//        public var size: CGSize
//        public var containerSize: CGSize
//        public var view: UIView {
//            return self._view
//        }
//        fileprivate let _view: _ZoomContentView
//        public init(containerSize: CGSize) {
//            self.size = CGSize()
//            self.containerSize = containerSize
//            self._view = _ZoomContentView(frame: CGRect(origin: CGPoint(), size: containerSize))
//            self.zoomScale = 1.0
//            super.init()
//        }
//    }
//
//    public var view: UIView {
//        return self._view
//    }
//    fileprivate let _view: _ZoomView
//
//
//    public let content: Content
//    public init(frame: CGRect) {
//        self.content = Content(containerSize: frame.size)
//        self._view = _ZoomView(frame: CGRect(origin: CGPoint(), size: frame.size))
//        super.init()
//        self._view.addSubview(self.content.view)
//        self._view._zoomViewDelegate = self
//    }
//}

//extension ZoomController : _ZoomViewDelegate {
//    func _zoomViewWillLayoutSubviews() {
////        if self.bounds.size != self.content.containerSize {
////            self.content.containerSize = self.bounds.size
////        }
//    }
//    func _zoomViewDidLayoutSubviews() {
//
//    }
//}
//
//extension ZoomController : UIScrollViewDelegate {
//    public func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
//
//    }
//
//    public func scrollViewDidEndScrollingAnimation(_ scrollView: UIScrollView) {
//
//    }
//
//    public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
//        return self.content.view
//    }
//
//    public func scrollViewWillBeginZooming(_ scrollView: UIScrollView, with view: UIView?) {
//
//    }
//
//    public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
//
//    }
//}
