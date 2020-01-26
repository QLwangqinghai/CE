//
//  DrawingContentController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingContentController: DrawingSliceController<CALayer> {
    
    public let view: DrawingBoardView

    public override init(pageContext: DrawingPageContext, domain: String, dataSource: DrawingContentDataSource) {
        self.view = DrawingBoardView(frame: pageContext.bounds)
        super.init(pageContext: pageContext, domain: domain, dataSource: dataSource)
    }
    
    public override func loadContent() -> CALayer {
        let view: CALayer = CALayer()
        view.frame = self.pageContext.bounds
        return view
    }
    
    public override func contentDidLoad(_ content: CALayer) {
        super.contentDidLoad(content)
        self.dataSource.prepare(slices: self.slices)
        
    }
    
}







