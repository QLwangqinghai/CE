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
fileprivate class _ZoomView: UIScrollView {
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
    
    public let content: Content
    fileprivate weak var _zoomViewDelegate: _ZoomViewDelegate?
    
    public override init(frame: CGRect) {
        self.content = Content(containerSize: frame.size)
        super.init(frame: frame)
    }
    
    required public init?(coder aDecoder: NSCoder) {
        self.content = Content(containerSize: CGSize())
        super.init(coder: aDecoder)
    }
    
    open override func layoutSubviews() {
        guard let delegate = self._zoomViewDelegate else {
            super.layoutSubviews()
            return
        }
        delegate._zoomViewDidLayoutSubviews()
        super.layoutSubviews()
        delegate._zoomViewDidLayoutSubviews()
    }
    
}


open class ZoomController: NSObject {

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
            return self._view
        }
        fileprivate let _view: ZoomContentView
        public init(containerSize: CGSize) {
            self.size = CGSize()
            self.containerSize = containerSize
            self._view = ZoomContentView(frame: CGRect(origin: CGPoint(), size: containerSize))
            self.zoomScale = 1.0
            super.init()
        }
    }
    
    public var view: UIView {
        return self._view
    }
    fileprivate let _view: _ZoomView

    
    public let content: Content
    //    open override var zoomScale: CGFloat {
    //        willSet(newValue) {}
    //        didSet(oldValue) {
    //
    //
    //        }
    //    }
    
    
    public init(frame: CGRect) {
        self.content = Content(containerSize: frame.size)
        self._view = _ZoomView(frame: CGRect(origin: CGPoint(), size: frame.size))
        super.init()
        self._view.addSubview(self.content.view)
        self._view._zoomViewDelegate = self
    }
}

extension ZoomController : _ZoomViewDelegate {
    func _zoomViewWillLayoutSubviews() {
//        if self.bounds.size != self.content.containerSize {
//            self.content.containerSize = self.bounds.size
//        }
    }
    func _zoomViewDidLayoutSubviews() {
        
    }
}

extension ZoomController : UIScrollViewDelegate {
    public func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
        
    }
    
    public func scrollViewDidEndScrollingAnimation(_ scrollView: UIScrollView) {
        
    }
    
    public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
        return self.content.view
    }
    
    public func scrollViewWillBeginZooming(_ scrollView: UIScrollView, with view: UIView?) {
        
    }
    
    public func scrollViewDidEndZooming(_ scrollView: UIScrollView, with view: UIView?, atScale scale: CGFloat) {
        
    }
}
