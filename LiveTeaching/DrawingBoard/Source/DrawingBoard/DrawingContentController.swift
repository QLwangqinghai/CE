//
//  DrawingContentController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingContentController: DrawingSliceController<DrawingBoardView> {
    public override func loadContent() -> DrawingBoardView {
        let view: DrawingBoardView = DrawingBoardView(frame: self.pageContext.bounds)
        return view
    }
    
    public override func contentDidLoad(_ content: DrawingBoardView) {
        super.contentDidLoad(content)
        self.dataSource.prepare(slices: self.slices)
    }
    
}







