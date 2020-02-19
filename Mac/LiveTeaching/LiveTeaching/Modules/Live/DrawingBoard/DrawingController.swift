//
//  DrawingController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
@_exported import CCFoundation
@_exported import Basic


public protocol DrawingHandle: class {
    var drawingView: DrawingView? { get }
    
    func willAppear(drawingView: DrawingView)
    func didAppear(drawingView: DrawingView)
}
public protocol DrawingSectionHandle: DrawingHandle {
    func hasPrev() -> Bool
    func hasNext() -> Bool
}

public class DrawingController: NSObject, DrawingViewDrawDelegate {
    public private(set) var drawingView: DrawingView? = nil

//    public private(set) var status: DrawingView.Status
//    public init(status: DrawingView.Status, bitmapLayout: Drawing.BitmapLayout, zoomScale: CGFloat, backgroundDataSource: DrawingContentDataSource, contentDataSource: DrawingContentDataSource) {
//        let pageContext = DrawingPageContext(ownerIdentifier: ownerIdentifier, domain: domain, contentSize: contentSize, offset: offset, bitmapLayout: bitmapLayout, zoomScale: zoomScale)
//        self.pageContext = pageContext
//        self.drawingBoardController = DrawingBoardController(pageContext: pageContext, size: Size(width: contentSize.width, height: contentSize.width))
//        self.backgroundController = DrawingBackgroundController(pageContext: pageContext, domain: "background", dataSource: backgroundDataSource)
//        self.contentController = DrawingContentController(pageContext: pageContext, domain: "content", dataSource: contentDataSource)
//        let observer = DrawingPageContext.Observer(identifier: pageContext.identifier)
//        pageContext.addObserver(observer)
//        self.observer = observer
//    }

    
    //MARK: DrawingViewDrawDelegate
    public func drawingViewDidBeginRecognize(_ view: DrawingView) {
        //开始识别
    }
    public func drawingViewDidRecognizedFailed(_ view: DrawingView) {
        //识别失败
    }
    public func drawingViewDidRecognizedSuccess(_ view: DrawingView) {
        //识别成功

    }

    //以下3个api是识别成功后的调用
    public func drawingView(_ view: DrawingView, beginDraw points:[TouchPoint]) {
        
    }
    public func drawingView(_ view: DrawingView, draw to:TouchPoint) {
        
    }
    public func drawingViewFinishDraw(_ view: DrawingView) {
        
    }

}

public class DrawingSectionController: NSObject {
    public private(set) var childs: [DrawingController]
    public private(set) var currentIndex: Int
    public var current: DrawingController {
        return self.childs[self.currentIndex]
    }
    public let identifier: String

    public init(identifier: String, childs: [DrawingController], currentIndex: Int) {
        assert(childs.count > 0)
        self.identifier = identifier
        self.childs = childs
        self.currentIndex = currentIndex
    }
    
    //TODO: fixme
    public func updateIndex(_ to: Int) throws {
        if to >= self.childs.count {
            throw NSError(domain: "", code: 0, userInfo: ["" : "没有下一页了"])
        } else if to < 0 {
            throw NSError(domain: "", code: 0, userInfo: ["" : "没有上一页了"])
        } else {
            self.currentIndex = to
        }
    }
    
    public func updateDataSource() {

    }
    deinit {
        
    }
    
}



public class DrawingContainerController: NSObject {
    public var frame: CGRect {
        didSet {
            self.container.frame = self.frame
        }
    }
    public private(set) var currentSection: DrawingSectionHandle?
    public let container: DrawingContainer
    let observer: StackView.Observer = StackView.Observer()

    public init(frame: CGRect, drawingSize: DrawingSize, contentHeightLimit: UInt32, bitmapLayout: Drawing.BitmapLayout) {
        self.frame = frame
        self.container = DrawingContainer(frame: frame, drawingSize: drawingSize, contentHeightLimit: contentHeightLimit, bitmapLayout: bitmapLayout)
    }
//    public override func loadContent() -> DrawingContainer {
//        let container: DrawingContainer = DrawingContainer(frame: self.frame)
//        container.addObserver(self.observer)
//        self.observer.didLayoutSubviews = {[weak self] (_ observer: StackView.Observer, _ view: StackView) in
//            guard let strongSelf = self else {
//                return
//            }
//            strongSelf.layoutSubviews()
//        }
//        return container
//    }
    
    public func updateCurrentSection(_ current: DrawingSectionHandle?) {
        
    }
    
    public func updateDataSource() {

    }
    
}


