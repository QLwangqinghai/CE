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
//    fileprivate class ZoomContentView: UIView {
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
        public override init(frame: CGRect) {
            super.init(frame: frame)
        }
        required public init?(coder aDecoder: NSCoder) {
            super.init(coder: aDecoder)
        }
    }
    
    fileprivate class ZoomContentView: UIView {
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
        fileprivate let contentView: ZoomContentView
        public init(containerSize: CGSize) {
            self.size = CGSize()
            self.containerSize = containerSize
            self.contentView = ZoomContentView(frame: CGRect(origin: CGPoint(), size: containerSize))
            self.zoomScale = 1.0
            super.init()
        }
    }
    private let _scrollView: _ZoomScrollView
    internal var scrollView: UIScrollView {
        return self._scrollView
    }
    public let contentView: UIView

    public let content: Content
    fileprivate weak var _zoomViewDelegate: _ZoomViewDelegate?
    
    public private(set) var rotate90CcwCount: UInt = 0
    
    private func resetScrollView() {
        let count = self.rotate90CcwCount
        if count != 0 {
            self._scrollView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(count)))
        } else {
            self._scrollView.transform = CGAffineTransform()
        }
    }
    
    public override init(frame: CGRect) {
        self.content = Content(containerSize: frame.size)
        self._scrollView = _ZoomScrollView(frame: CGRect(origin: CGPoint(), size: frame.size))
        self.contentView = ZoomContentView()
        super.init(frame: frame)
        
        self.addSubview(self._scrollView)
        self._scrollView.addSubview(self.contentView)
        self._scrollView.backgroundColor = UIColor.green
    }
    
    required public init?(coder aDecoder: NSCoder) {
        self.content = Content(containerSize: CGSize())
        self._scrollView = _ZoomScrollView()
        self.contentView = ZoomContentView()
        super.init(coder: aDecoder)
        self.addSubview(self._scrollView)
        self._scrollView.addSubview(self.contentView)
    }
    
    open override func layoutSubviews() {
//        var frame = self.bounds
//        if self.rotate90CcwCount % 2 != 0 {
//            let t = frame.size.width
//            frame.size.width = frame.size.height
//            frame.size.height = t
//        }
//        self.scrollView.frame = frame
        super.layoutSubviews()
    }
    
    open func rotate90Ccw(animate: Bool) {
        self .updateRotate90CcwCount(self.rotate90CcwCount + 1, animate: animate)
    }
    
    open func updateRotate90CcwCount(_ count: UInt, animate: Bool) {
        let value = count % 4
        if self.rotate90CcwCount != value {
            self.rotate90CcwCount = value
            self._scrollView.transform = CGAffineTransform.identity
            let bounds = self.bounds
            var frame = CGRect()
            if value % 2 != 0 {
                frame.size.width = bounds.size.height
                frame.size.height = bounds.size.width
            } else {
                frame.size.width = bounds.size.width
                frame.size.height = bounds.size.height
            }

            frame.origin.x = (bounds.size.width - frame.size.width) / 2
            frame.origin.y = (bounds.size.height - frame.size.height) / 2
            
            self._scrollView.frame = frame
            if value != 0 {
                self._scrollView.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi / 2 * Double(value)))
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
//    fileprivate class ZoomContentView: UIView {
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
//        fileprivate let _view: ZoomContentView
//        public init(containerSize: CGSize) {
//            self.size = CGSize()
//            self.containerSize = containerSize
//            self._view = ZoomContentView(frame: CGRect(origin: CGPoint(), size: containerSize))
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
