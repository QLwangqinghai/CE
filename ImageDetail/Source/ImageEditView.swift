//
//  ImageEditView.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

open class PanGestureRecognizer: UIPanGestureRecognizer {
    
    
    
}



//open class ScrollView: UIScrollView {
//    
////    override var panGestureRecognizer: UIPanGestureRecognizer {
////        var a = super.panGestureRecognizer
////        return UIPanGestureRecognizer.init(target: a.target, action: <#T##Selector?#>)
////
////
////    }
//    
//    public override init(frame: CGRect) {
//        super.init(frame: frame)
//    }
//    
//    required public init?(coder aDecoder: NSCoder) {
//        super.init(coder: aDecoder)
//    }
//
//    open override func gestureRecognizerShouldBegin(_ gestureRecognizer: UIGestureRecognizer) -> Bool {
//        return true
//    }
//
//    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer) -> Bool {
//        return true
//    }
//    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive touch: UITouch) -> Bool {
//        
//        return true
//    }
//    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive press: UIPress) -> Bool {
//        
//        
//        return true
//    }
//    
//    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRequireFailureOf otherGestureRecognizer: UIGestureRecognizer) -> Bool {
//        return false
//    }
//
//    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldBeRequiredToFailBy otherGestureRecognizer: UIGestureRecognizer) -> Bool {
//        return false
//    }
//    
//}


//@objc fileprivate protocol ZoomViewContentDelegate : NSObjectProtocol {
//    @objc optional func willSetView(content: ZoomView.Content, newView: UIView?)
//    @objc optional func didSetView(content: ZoomView.Content, oldView: UIView?)
//}

//open class ZoomView: UIScrollView {
//    fileprivate class _ZoomContentView: UIView {
//        public override init(frame: CGRect) {
//            super.init(frame: frame)
//        }
//
//        required public init?(coder aDecoder: NSCoder) {
//            self.content = Content(containerSize: CGSize())
//            super.init(coder: aDecoder)
//            self.content.delegate = self
//        }
//    }
//
//
//
//
//    public class Content: NSObject {
//        fileprivate var delegate: ZoomViewContentDelegate?
//
//        public var size: CGSize
//        public var containerSize: CGSize
//
//        public var view: UIView? {
//            willSet(newValue) {
//                if let delegate = self.delegate {
//                    delegate.willSetView?(content: self, newView: newValue)
//                }
//            }
//            didSet(oldValue) {
//                if let delegate = self.delegate {
//                    delegate.didSetView?(content: self, oldView: oldValue)
//                }
//            }
//        }
//        public init(containerSize: CGSize) {
//            self.size = CGSize()
//            self.containerSize = containerSize
//            super.init()
//        }
//    }
//
//    public let content: Content
//    open override var zoomScale: CGFloat {
//        willSet(newValue) {}
//        didSet(oldValue) {
//
//
//        }
//    }
//
//
//    public override init(frame: CGRect) {
//        self.content = Content(containerSize: frame.size)
//        super.init(frame: frame)
//        self.content.delegate = self
//    }
//
//    required public init?(coder aDecoder: NSCoder) {
//        self.content = Content(containerSize: CGSize())
//        super.init(coder: aDecoder)
//        self.content.delegate = self
//    }
//
//    open override func layoutSubviews() {
//        super.layoutSubviews()
//        if self.bounds.size != self.content.containerSize {
//            self.content.containerSize = self.bounds.size
//        }
//
//    }
//
//
//}
//
//extension ZoomView : ZoomViewContentDelegate {
//
//
//}

fileprivate protocol _ZoomViewDelegate: class {
    func _zoomViewWillLayoutSubviews()
    func _zoomViewDidLayoutSubviews()
}

public protocol OrientationContent: class {
    var orientationContentView: UIView {get}
    var orientationSafeAreaInsets: UIEdgeInsets { get set}
}

open class OrientationView: UIView {
    public enum Direction: Int {
        case anticlockwise = -1
        case clockwise = 1
    }
    
    public var content: OrientationContent? {
        willSet {
            guard let old = self.content else {
                return
            }
            old.orientationSafeAreaInsets = UIEdgeInsets()
            if old.orientationContentView.superview == self {
                old.orientationContentView.removeFromSuperview()
            }
        }
        didSet {
            guard let content = self.content else {
                return
            }
            self.addSubview(content.orientationContentView)
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
        var contentViewBounds = contentView.bounds
        let swapXY = value % 2 != 0
        if swapXY {
            contentViewBounds.size.width = bounds.size.height
            contentViewBounds.size.height = bounds.size.width
        } else {
            contentViewBounds.size.width = bounds.size.width
            contentViewBounds.size.height = bounds.size.height
        }
        contentView.bounds = contentViewBounds
        var center = CGPoint()
        center.x = (bounds.size.width) / 2
        center.y = (bounds.size.height) / 2
        contentView.center = center
        contentView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(value)))
        
        var safeAreaInsets = self.safeAreaInsets
        let array: [CGFloat] = [safeAreaInsets.top, safeAreaInsets.left, safeAreaInsets.bottom, safeAreaInsets.right]
        safeAreaInsets.top = array[Int(value % 4)]
        safeAreaInsets.left = array[Int((value + 1) % 4)]
        safeAreaInsets.bottom = array[Int((value + 2) % 4)]
        safeAreaInsets.right = array[Int((value + 3) % 4)]
        if content.orientationSafeAreaInsets != safeAreaInsets {
            content.orientationSafeAreaInsets = safeAreaInsets
        }
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
        var orientationSafeAreaInsets: UIEdgeInsets = UIEdgeInsets()
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
//        open override var zoomScale: CGFloat {
//            willSet(newValue) {}
//            didSet(oldValue) {
//                if let closure = self.onZoomScaleChanged {
//                    closure(self, oldValue)
//                }
//            }
//        }
        
//        @available(iOS 11.0, *)
//        open override func adjustedContentInsetDidChange() {
//            super.adjustedContentInsetDidChange()
//            print("\(self) adjustedContentInset:\(self.adjustedContentInset)")
//        }
//        @available(iOS 11.0, *)
//        open override func safeAreaInsetsDidChange() {
//            super.safeAreaInsetsDidChange()
//            print("\(self) safeAreaInsets:\(self.safeAreaInsets)")
//        }
    }
    
    fileprivate class _ZoomContentView: UIView {
        public override init(frame: CGRect) {
            super.init(frame: frame)
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
        }
    }
    
    public class Content: NSObject {
        public var zoomScale: CGFloat
        public var size: CGSize
        public var containerSize: CGSize
        public var view: UIView {
            return self.contentView
        }
        fileprivate let contentView: _ZoomContentView
        public init(containerSize: CGSize) {
            self.size = CGSize()
            self.containerSize = containerSize
            self.contentView = _ZoomContentView(frame: CGRect(origin: CGPoint(), size: containerSize))
            self.zoomScale = 1.0
            super.init()
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
    
    
    
    private func resetScrollView() {
        let value = self.rotate90CcwCount
        let scrollView: _ZoomScrollView  = self._scrollView
        let bounds = self.bounds
        var scrollViewBounds = scrollView.bounds
        let swapXY = value % 2 != 0
        if swapXY {
            scrollViewBounds.size.width = bounds.size.height
            scrollViewBounds.size.height = bounds.size.width
        } else {
            scrollViewBounds.size.width = bounds.size.width
            scrollViewBounds.size.height = bounds.size.height
        }
        scrollView.bounds = scrollViewBounds
        var center = CGPoint()
        center.x = (bounds.size.width) / 2
        center.y = (bounds.size.height) / 2
        scrollView.center = center
        if value % 4 != 0 {
            scrollView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(value) * -1))
        } else {
            scrollView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi * 2 * -1))
        }
        
        var safeAreaInsets = self.safeAreaInsets
        let array: [CGFloat] = [safeAreaInsets.top, safeAreaInsets.left, safeAreaInsets.bottom, safeAreaInsets.right]
        safeAreaInsets.top = array[Int(value % 4)]
        safeAreaInsets.left = array[Int((value + 1) % 4)]
        safeAreaInsets.bottom = array[Int((value + 2) % 4)]
        safeAreaInsets.right = array[Int((value + 3) % 4)]
        
        let contentView: _ZoomContentView = self._contentView
        let contentSize = contentView.frame.size
        var paddingX: CGFloat = scrollViewBounds.size.width - safeAreaInsets.left - safeAreaInsets.right - contentSize.width
        var paddingY: CGFloat = scrollViewBounds.size.height - safeAreaInsets.top - safeAreaInsets.bottom - contentSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        scrollView.update(safeInset: safeAreaInsets, paddingInset: paddingInset)
        

    }
    
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
