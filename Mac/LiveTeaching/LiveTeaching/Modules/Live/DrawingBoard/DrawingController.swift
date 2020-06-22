//
//  DrawingController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
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

public class DrawingBoardController: NSObject, DrawingViewDrawDelegate {
    public let drawingBoard: DrawingBoard
    
    public var frame: CGRect {
        didSet {
            self.drawingBoard.frame = self.frame
        }
    }
    let observer: StackView.Observer = StackView.Observer()

    public init(frame: CGRect, session: String, size: Size, contentSize: Size) {
        self.frame = frame
        self.drawingBoard = DrawingBoard(frame: frame, session: session, size: size, contentSize: contentSize)
    }

    
    //MARK: DrawingViewDrawDelegate

    public func drawingView(_ view: DrawingView, shouldBeginRecognizeAt point: TouchPoint) -> AnyHashable? {
        //即将识别

        return nil
    }
    
    public func drawingView(_ view: DrawingView, didBeginRecognize context: TouchPointCollection) {
        //开始识别

    }
    
    public func drawingView(_ view: DrawingView, didRecognizedFailed context: TouchPointCollection) {
        //识别失败
    }
    
    public func drawingView(_ view: DrawingView, didRecognizedSuccess context: TouchPointCollection) {
        //识别成功

    }
    
    
    //以下2个api是识别成功后的调用
    public func drawingView(_ view: DrawingView, draw context: TouchPointCollection) {
        //内容有变化
        
    }
    
    public func drawingView(_ view: DrawingView, finish context: TouchPointCollection) {
        //结束
    }
    
    
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


}

//public class DrawingSectionController: NSObject {
//    public private(set) var childs: [DrawingController]
//    public private(set) var currentIndex: Int
//    public var current: DrawingController {
//        return self.childs[self.currentIndex]
//    }
//    public let identifier: String
//
//    public init(identifier: String, childs: [DrawingController], currentIndex: Int) {
//        assert(childs.count > 0)
//        self.identifier = identifier
//        self.childs = childs
//        self.currentIndex = currentIndex
//    }
//    
//    //TODO: fixme
//    public func updateIndex(_ to: Int) throws {
//        if to >= self.childs.count {
//            throw NSError(domain: "", code: 0, userInfo: ["" : "没有下一页了"])
//        } else if to < 0 {
//            throw NSError(domain: "", code: 0, userInfo: ["" : "没有上一页了"])
//        } else {
//            self.currentIndex = to
//        }
//    }
//    
//    public func updateDataSource() {
//
//    }
//    deinit {
//        
//    }
//    
//}
//
//public class DrawingContainerController: NSObject {
//    public var frame: CGRect {
//        didSet {
//            self.window.frame = self.frame
//        }
//    }
//    public private(set) var currentSection: DrawingSectionHandle?
//    public let window: DrawingBoard
//    let observer: StackView.Observer = StackView.Observer()
//
//    public init(frame: CGRect, contentSize: Size) {
//        self.frame = frame
//        self.window = DrawingBoard(frame: frame, contentSize: contentSize)
//    }
////    public override func loadContent() -> DrawingContainer {
////        let container: DrawingContainer = DrawingContainer(frame: self.frame)
////        container.addObserver(self.observer)
////        self.observer.didLayoutSubviews = {[weak self] (_ observer: StackView.Observer, _ view: StackView) in
////            guard let strongSelf = self else {
////                return
////            }
////            strongSelf.layoutSubviews()
////        }
////        return container
////    }
//    
//    public func updateCurrentSection(_ current: DrawingSectionHandle?) {
//        
//    }
//    
//    public func updateDataSource() {
//
//    }
//    
//}


