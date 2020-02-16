//
//  DrawingBoardController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public class DrawingBoardController: BaseController<DrawingBoard> {
    public static let zPosition: CGFloat = 100

    public private(set) var sequence: UInt64 = 0

    public let size: Size
    public let layer: CALayer
    public let pageContext: DrawingPageContext


    public init(pageContext: DrawingPageContext, size: Size) {
        self.pageContext = pageContext
        self.size = size
        self.layer = CALayer()
        super.init()
    }
    
    
    public override func loadContent() -> DrawingBoard {
        let broad: DrawingBoard = DrawingBoard(size: self.size, bitmapLayout: self.pageContext.bitmapLayout)
        return broad
    }
    
    public override func contentDidLoad(_ content: DrawingBoard) {
        super.contentDidLoad(content)
    }
    
    public func beginDrawing(borad: DrawingBoard) throws {
        let offset = self.pageContext.offset
        try borad.begin(controller: self, offset: offset)
    }
    
    public func pointBuffer(y: Int32) -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer.allocate(byteCount: 10, alignment: CCGetCachelineSize())
    }
}

