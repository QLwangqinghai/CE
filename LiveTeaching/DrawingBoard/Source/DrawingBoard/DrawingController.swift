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
    public let pageContext: DrawingPageContext
    
    public init(ownerIdentifier: String, domain: String, contentSize: Size, offset: Int32, bitmapLayout: BitmapLayout, zoomScale: CGFloat, backgroundDataSource: DrawingContentDataSource) {
        let pageContext = DrawingPageContext(ownerIdentifier: ownerIdentifier, domain: domain, contentSize: contentSize, offset: offset, bitmapLayout: bitmapLayout, zoomScale: zoomScale)
        self.pageContext = pageContext
        self.drawingBoardController = DrawingBoardController(pageContext: pageContext, size: Size(width: contentSize.width, height: contentSize.width))
        self.backgroundController = DrawingBackgroundController(pageContext: pageContext, domain: "background", dataSource: backgroundDataSource)
    }
    
    public override func loadContent() -> DrawingContainer {
        let container: DrawingContainer = DrawingContainer(frame: self.pageContext.bounds)
        return container
    }
    
    public override func contentDidLoad(_ content: DrawingContainer) {
        super.contentDidLoad(content)
    }
    
}

public class DrawingSectionController: BaseController<UIStackView> {
    public private(set) var childs: [DrawingController]
    public private(set) var frame: CGRect
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
        return container
    }
    
    public override func contentDidLoad(_ content: UIStackView) {
        super.contentDidLoad(content)
        let top: DrawingController = self.childs[self.childs.count - 1]
        content.addSubview(top.content)
        top.pageContext.updateIsHidden(false)
    }
    
    public func updateDataSource() {

    }
    deinit {
        for child in self.childs {
            child.pageContext.updateIsHidden(true)
        }
    }
    
}










