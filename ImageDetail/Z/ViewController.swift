//
//  ViewController.swift
//  Z
//
//  Created by vector on 2019/10/11.
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

open class ZoomView: UIView {
    fileprivate class _ZoomHandler: NSObject, UIScrollViewDelegate {
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
            guard let aview = view else {
                return
            }
            print("scrollViewDidEndZooming \(scrollView) at:\(aview) scale:\(scale)")
        }

        public func scrollViewDidZoom(_ scrollView: UIScrollView) {
            print("scrollViewDidZoom \(scrollView)")
        }
    }
    
    private let _scrollView: UIScrollView
    private let _scrollZoomHandler: _ZoomHandler
    private let _contentView: UIView

    private func resetContentInset() {
        let old = self._scrollView.contentInset
        let new = UIUtil.add(self.orientationInset, self.paddingInset)
        self._scrollView.contentInset = new
//        var offset = self.contentOffset
//        offset.x = offset.x - (new.left - old.left)
//        offset.y = offset.y - (new.top - old.top)
//        self.contentOffset = offset
    }
    
    public var orientationInset: UIEdgeInsets = UIEdgeInsets() {
        didSet {
            self.resetContentInset()
        }
    }
    public fileprivate(set) var paddingInset: UIEdgeInsets = UIEdgeInsets() {
        didSet {
            self.resetContentInset()
        }
    }
    
    private func _prepareBase() {
        var scrollViewFrame = self.bounds
        scrollViewFrame.origin = CGPoint()
        if #available(iOS 11.0, *) {
            self._scrollView.contentInsetAdjustmentBehavior = .never
        }
        self._scrollView.frame = scrollViewFrame
        self.addSubview(self._scrollView)
        
        
        self._scrollZoomHandler.scrollView = self._scrollView
        self._scrollZoomHandler.viewForZooming = self._contentView
        self._scrollView.delegate = self._scrollZoomHandler
    }
    
    public override init(frame: CGRect) {
        self._scrollView = UIScrollView()
        self._scrollZoomHandler = _ZoomHandler()
        self._contentView = UIView()
        super.init(frame: frame)
        self._prepareBase()
    }
    required public init?(coder aDecoder: NSCoder) {
        self._scrollView = UIScrollView()
        self._scrollZoomHandler = _ZoomHandler()
        self._contentView = UIView()
        super.init(coder: aDecoder)
        self._prepareBase()
    }
}


class ViewController: UIViewController, UIScrollViewDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        
        let scrollView = ZoomView(frame: self.view.bounds)
        scrollView.delegate = self
        scrollView.contentInset = UIEdgeInsets(top: 0, left: 10, bottom: 0, right: 0)
        scrollView.backgroundColor = UIColor.green
        self.view.addSubview(scrollView)
        scrollView.contentSize = CGSize(width: 200, height: 100)
        scrollView.minimumZoomScale = 0.2
        scrollView.maximumZoomScale = 30
        
        let imageView = UIImageView(frame: CGRect(origin: CGPoint(x: 0, y: 0), size: CGSize(width: 200, height: 100)))
        imageView.image = UIImage(named: "10.jpg")

        scrollView.addSubview(imageView)
        imageView.backgroundColor = UIColor.red
        
        scrollView.delegate = self
        
        let zoomSize = scrollView.contentSize
        let scrollSize = scrollView.bounds.size
        let safeInset = scrollView.orientationInset
        var paddingX: CGFloat = scrollSize.width - safeInset.left - safeInset.right - zoomSize.width
        var paddingY: CGFloat = scrollSize.height - safeInset.top - safeInset.bottom - zoomSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        scrollView.paddingInset = paddingInset
    }

    public func viewForZooming(in scrollView: UIScrollView) -> UIView? {
        return scrollView.subviews.first
    }
    public func scrollViewDidZoom(_ scrollView1: UIScrollView) {
        print("scrollViewDidZoom \(scrollView1) ")
        guard let scrollView = scrollView1 as? _ZoomScrollView else {
            return
        }
        let zoomSize = scrollView.contentSize
        let scrollSize = scrollView.bounds.size
        let safeInset = scrollView.orientationInset
        var paddingX: CGFloat = scrollSize.width - safeInset.left - safeInset.right - zoomSize.width
        var paddingY: CGFloat = scrollSize.height - safeInset.top - safeInset.bottom - zoomSize.height
        if paddingX < 0 {
            paddingX = 0
        }
        if paddingY < 0 {
            paddingY = 0
        }
        let paddingInset = UIEdgeInsets(top: paddingY / 2, left: paddingX / 2, bottom: paddingY / 2, right: paddingX / 2)
        scrollView.paddingInset = paddingInset
    }
}

