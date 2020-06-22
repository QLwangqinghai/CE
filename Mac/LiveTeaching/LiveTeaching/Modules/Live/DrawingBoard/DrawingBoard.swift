//
//  DrawingWindow.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
import UIFoundation


public class DrawingBoard: StackView {

    public let drawingView: DrawingView
    
    //承载内容 DrawingView
    public let zoomConainer: StackView

    
    //动画  多媒体层， 跟session无关的数据
    public let topForeground: ViewContainer

    //菜单层
    public let operationView: ViewContainer
    
    public let contentSize: Size
    
    public init(frame: CGRect, session: String, size: Size, contentSize: Size) {
        self.contentSize = contentSize
        
        let bounds = CGRect(origin: CGPoint(), size: frame.size)
        let zoomConainer: StackView = StackView(frame: bounds)
        let drawingView: DrawingView = DrawingView(frame:bounds, session: session, size: size, contentSize: contentSize)
        let topForeground: ViewContainer = ViewContainer(frame: bounds)
        zoomConainer.addSubview(drawingView)
        zoomConainer.addSubview(topForeground)

        self.zoomConainer = zoomConainer
        self.drawingView = drawingView

        self.topForeground = topForeground
        self.operationView = ViewContainer(frame: bounds)
        
        super.init(frame: frame)
        self.addSubview(zoomConainer)
        self.addSubview(self.operationView)
        self.layout()
    }
    
    required init(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    open override func layout() {
        super.layout()
        let bounds = self.bounds
        let contentBounds = CGRect(origin: CGPoint(), size: self.contentSize.cgSize)
        self.zoomConainer.bounds = contentBounds
        self.zoomConainer.transform = CGAffineTransform.identity.scaledBy(x: bounds.size.width / contentBounds.size.width, y: bounds.size.height / contentBounds.size.height)
        self.zoomConainer.center = CGPoint(x: bounds.size.width/2, y: bounds.size.height/2)
        self.operationView.frame = CGRect(origin: CGPoint(), size: bounds.size)
    }
}


