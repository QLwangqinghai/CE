//
//  DrawingController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public class DrawingController: BaseController<DrawingContainer> {
    
    public let backgroundController: DrawingBackgroundController
    public let drawingBoardController: DrawingBoardController
    public let contentController: DrawingContentController
    
    public let pageContext: DrawingPageContext
    public let observer: DrawingPageContext.Observer

    public init(ownerIdentifier: String, domain: String, contentSize: Size, offset: Int32, bitmapLayout: BitmapLayout, zoomScale: CGFloat, backgroundDataSource: DrawingContentDataSource, contentDataSource: DrawingContentDataSource) {
        let pageContext = DrawingPageContext(ownerIdentifier: ownerIdentifier, domain: domain, contentSize: contentSize, offset: offset, bitmapLayout: bitmapLayout, zoomScale: zoomScale)
        self.pageContext = pageContext
        self.drawingBoardController = DrawingBoardController(pageContext: pageContext, size: Size(width: contentSize.width, height: contentSize.width))
        self.backgroundController = DrawingBackgroundController(pageContext: pageContext, domain: "background", dataSource: backgroundDataSource)
        self.contentController = DrawingContentController(pageContext: pageContext, domain: "content", dataSource: contentDataSource)

        let observer = DrawingPageContext.Observer(identifier: pageContext.identifier)
        pageContext.addObserver(observer)
        self.observer = observer
    }
    
    public override func loadContent() -> DrawingContainer {
        let container: DrawingContainer = DrawingContainer(frame: self.pageContext.bounds)
        return container
    }
    
    public override func contentDidLoad(_ content: DrawingContainer) {
        super.contentDidLoad(content)
        
        content.addSubview(self.contentController.view)
//        content.layer.addSublayer(self.backgroundController.content)
//        content.layer.addSublayer(self.contentController.content)

        self.observer.didUpdateOffset = {[weak self] (_ observer: DrawingPageContext.Observer, _ context: DrawingPageContext, _ from: Int32, _ to: Int32) in
            guard let `self` = self else {
                return
            }
            self.content.bounds.origin.y = CGFloat(to) / self.pageContext.zoomScale
        }
        self.pageContext.addObserver(self.observer)
        
        
        self.content.bounds.origin.y = CGFloat(self.pageContext.offset) / self.pageContext.zoomScale
    }
    deinit {
        self.pageContext.removeObserver(self.observer)
    }
}

public class DrawingSectionController: BaseController<UIStackView> {
    public private(set) var childs: [DrawingController]
    public var frame: CGRect {
        didSet {
            self.content.frame = self.frame
        }
    }
    public private(set) var currentIndex: Int
    public var current: DrawingController {
        return self.childs[self.currentIndex]
    }
    public let identifier: String

    public private(set) var isHidden: Bool {
        didSet(old) {
            if old != self.isHidden {
                self.current.pageContext.updateIsHidden(self.isHidden)
            }
        }
    }
    
    public init(identifier: String, childs: [DrawingController], currentIndex: Int, frame: CGRect) {
        assert(childs.count > 0)
        self.identifier = identifier
        self.childs = childs
        self.currentIndex = currentIndex
        self.frame = frame
        self.isHidden = true
        for child in childs {
            child.pageContext.updateIsHidden(true)
        }
    }
    
    public override func loadContent() -> UIStackView {
        let container: UIStackView = UIStackView(frame: self.frame)
        container.clipsToBounds = true
        return container
    }
    
    public override func contentDidLoad(_ content: UIStackView) {
        super.contentDidLoad(content)
        let top: DrawingController = self.childs[self.childs.count - 1]
        content.addSubview(top.content)
        top.pageContext.updateIsHidden(false)
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
        for child in self.childs {
            child.pageContext.updateIsHidden(true)
        }
    }
    
}










