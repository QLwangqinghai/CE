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
open class ZoomView: UIView {
    internal class _ZoomScrollView: UIScrollView {
//        open var safeContentInset: UIEdgeInsets { get }

        fileprivate private(set) var safeInset: UIEdgeInsets = UIEdgeInsets()
        fileprivate private(set) var paddingInset: UIEdgeInsets = UIEdgeInsets()

        internal func update(safeInset: UIEdgeInsets, paddingInset: UIEdgeInsets) {
            self.safeInset = safeInset
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
    public let content: Content
    fileprivate weak var _zoomViewDelegate: _ZoomViewDelegate?
    
    public private(set) var rotate90CcwCount: UInt = 0
    
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
        self.content = Content(containerSize: frame.size)
        self._scrollView = _ZoomScrollView(frame: CGRect(origin: CGPoint(), size: frame.size))
        self._contentView = _ZoomContentView()
        super.init(frame: frame)
        
        self.addSubview(self._scrollView)
        self.clipsToBounds = true
        self._scrollView.addSubview(self._contentView)
        self._scrollView.backgroundColor = UIColor.green
    }
    
    required public init?(coder aDecoder: NSCoder) {
        self.content = Content(containerSize: CGSize())
        self._scrollView = _ZoomScrollView()
        self._contentView = _ZoomContentView()
        super.init(coder: aDecoder)
        self.clipsToBounds = true
        self.addSubview(self._scrollView)
        self._scrollView.addSubview(self._contentView)
    }
    
    open override func layoutSubviews() {
        self.resetScrollView()
        super.layoutSubviews()
    }

    @available(iOS 11.0, *)
    open override func safeAreaInsetsDidChange() {
        super.safeAreaInsetsDidChange()
        self.resetScrollView()
//        print("\(self) safeAreaInsets:\(self.safeAreaInsets)")
    }
    
    
    open func rotate90Ccw(completion: ((Bool) -> Void)? = nil) {
        self.updateRotate90CcwCount(self.rotate90CcwCount + 1, completion:completion)
    }
    
    open func updateRotate90CcwCount(_ count: UInt, completion: ((Bool) -> Void)? = nil) {
        let value = count % 4
        if self.rotate90CcwCount != value {
            self.rotate90CcwCount = value
            if let closure = completion {
                UIView.animate(withDuration: 0.2, animations: {
                    self.resetScrollView()
                }) { (result) in
                    print("animate result:\(result)")
                    closure(result)
                }
            } else {
                self.resetScrollView()
            }
        }
    }
    
    
    
}

extension ZoomView : UIScrollViewDelegate {
    public func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
        
    }
    
    public func scrollViewDidEndScrollingAnimation(_ scrollView: UIScrollView) {
        
    }
    
    public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
        if scrollView == self._scrollView {
            return self.content.view
        } else {
            return nil
        }
    }
    
    public func scrollViewWillBeginZooming(_ scrollView: UIScrollView, with view: UIView?) {
        
    }
    
    public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
        
    }
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
