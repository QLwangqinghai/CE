//
//  DrawingWindow.swift
//  LiveTeaching
//
//  Created by vector on 2020/3/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
import UIFoundation

public class DrawingContentView: StackView {
    fileprivate var foreground: UIView?

    public override func addSubview(_ view: UIView) {
        super.addSubview(view)
        if let foreground = self.foreground {
            if foreground.superview == self {
                self.bringSubviewToFront(foreground)
            }
        }
    }
}

public class DrawingWindow: StackView {
    
    //承载内容
    public let content: DrawingContentView
    public var drawingView: TiledDrawingView? {
        didSet(old) {
            if old !== self.drawingView {
                if let oldValue = old {
                    if oldValue.superview == self.content {
                        oldValue.removeFromSuperview()
                    }
                }
                if let newValue = self.drawingView {
                    self.content.addSubview(newValue)
                    let contentBounds = CGRect(origin: CGPoint(), size: self.contentSize.cgSize)
                    newValue.frame = contentBounds
                }
            }
        }
    }
    
    //动画  多媒体层
    public let topForeground: ViewContainer

    //菜单层
    public let operationView: ViewContainer
    
    public let contentSize: Size

    public init(frame: CGRect, contentSize: Size) {
        self.contentSize = contentSize
        let contentBounds = CGRect(origin: CGPoint(), size: contentSize.cgSize)
        let content: DrawingContentView = DrawingContentView(frame: contentBounds)
        
        let topForeground: ViewContainer = ViewContainer(frame: contentBounds)
        self.topForeground = topForeground
        self.operationView = ViewContainer(frame: contentBounds)
        self.content = content
        super.init(frame: frame)
        self.addSubview(content)
        self.addSubview(self.operationView)
        self.content.addSubview(topForeground)
        self.content.foreground = topForeground
        self.layout()
    }
    
    required init(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    open override func layout() {
        super.layout()
        let bounds = self.bounds
        let contentBounds = CGRect(origin: CGPoint(), size: self.contentSize.cgSize)
        self.content.bounds = contentBounds
        self.content.transform = CGAffineTransform.identity.scaledBy(x: bounds.size.width / contentBounds.size.width, y: bounds.size.height / contentBounds.size.height)
        self.content.center = CGPoint(x: bounds.size.width/2, y: bounds.size.height/2)
        self.operationView.frame = CGRect(origin: CGPoint(), size: bounds.size)
    }
}

