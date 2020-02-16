//
//  DrawingContext.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/17.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public class DrawingContext {
    public let drawingSize: DrawingSize
    public let drawingView: DrawingView
    public let operationView: ViewContainer
    public init(drawingSize: DrawingSize) {
        self.drawingSize = drawingSize
        self.drawingView = DrawingView(frame: CGRect(origin: CGPoint(), size: drawingSize.cgSize))
        self.operationView = ViewContainer(frame: CGRect())
    }
}
