//
//  DrawingPageContext.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingPageContext {
    public let identifier: String
    public let contentSize: Size
    public let zoomScale: CGFloat
    public let bounds: CGRect

    
    public private(set) var offset: Int32
    public let bitmapLayout: BitmapLayout

    public init(identifier: String, contentSize: Size, offset: Int32, bitmapLayout: BitmapLayout, zoomScale: CGFloat) {
        self.zoomScale = zoomScale
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.contentSize = contentSize
        self.offset = offset
        self.bounds = CGRect(x: 0, y: 0, width: CGFloat(contentSize.width) / zoomScale, height: CGFloat(contentSize.height) / zoomScale)
    }
}
