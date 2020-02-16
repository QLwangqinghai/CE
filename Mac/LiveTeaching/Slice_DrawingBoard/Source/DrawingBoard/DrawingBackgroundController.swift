//
//  DrawingBackgroundController.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingBackgroundController: DrawingSliceController<CALayer> {
    public override func loadContent() -> CALayer {
        let view: CALayer = CALayer()
        view.frame = self.pageContext.bounds
        return view
    }
    
    public override func contentDidLoad(_ view: CALayer) {
        super.contentDidLoad(content)
        self.dataSource.prepare(slices: self.slices)
    }
}
