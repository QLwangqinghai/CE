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

    private func resetContentInset() {
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
            self.resetContentInset()
        }
    }
    public fileprivate(set) var paddingInset: UIEdgeInsets = UIEdgeInsets() {
        didSet {
            self.resetContentInset()
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
            self._scrollView.contentInsetAdjustmentBehavior = .never
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
        guard scrollSize.width > 0 && scrollSize.width > 0 else {
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
        let scrollView = self._scrollView
        let contentView = self.contentView
        scrollView.minimumZoomScale = minimumZoomScale
        scrollView.maximumZoomScale = maximumZoomScale
        scrollView.zoomScale = zoomScale
        scrollView.contentSize = size
        contentView.bounds.size = size
        contentView.center = CGPoint(x: size.width / 2, y: size.height / 2)
    }
}


class ViewController: UIViewController, UIScrollViewDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let zoomView = ZoomView(frame: self.view.bounds)
        zoomView.backgroundColor = UIColor.green
        self.view.addSubview(zoomView)
        
//        zoomView.resetContent(size: CGSize(width: 200, height: 100), zoomScale: 1.0, minimumZoomScale: 0.2, maximumZoomScale: 30)
        
        let image: UIImage = UIImage(named: "10.jpg")!
        let imageSize = image.size
        
        let imageView = UIImageView(frame: CGRect(origin: CGPoint(x: 0, y: 0), size: imageSize))
        imageView.image = image
        zoomView.contentView.addSubview(imageView)
        imageView.backgroundColor = UIColor.red
        zoomView.resetContent(size: imageSize)
    }

}

