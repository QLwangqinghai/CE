//
//  DrawingController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public class DrawingController: BaseController<DrawingContainer> {
    public let drawingBoardController: DrawingBoardController
    public let identifier: String
    
    public let pageContext: DrawingPageContext
    
    public private(set) var dataSource: DrawingContentDataSource

    public init(identifier: String, contentSize: Size, offset: Int32, bitmapLayout: BitmapLayout, zoomScale: CGFloat, dataSource: DrawingContentDataSource) {
        let pageContext = DrawingPageContext(identifier: identifier, contentSize: contentSize, offset: offset, bitmapLayout: bitmapLayout, zoomScale: zoomScale)
        self.pageContext = pageContext
        self.drawingBoardController = DrawingBoardController(pageContext: pageContext, size: Size(width: contentSize.width, height: contentSize.width))
        self.dataSource = dataSource
        self.identifier = identifier
    }
    
    public override func loadContent() -> DrawingContainer {
        let container: DrawingContainer = DrawingContainer(frame: self.pageContext.bounds)
        return container
    }
    
    public override func contentDidLoad(_ content: DrawingContainer) {
        super.contentDidLoad(content)
    }
    
    public func updateDataSource() {

    }
    
}










