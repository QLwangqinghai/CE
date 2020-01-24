//
//  DrawingContainer.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingContainer: UIStackView {
    public let drawingBoard: DrawingBoardView
    
    public override init(frame: CGRect) {
        self.drawingBoard = DrawingBoardView(frame: CGRect())
        super.init(frame: frame)
        self.addSubview(self.drawingBoard)
        self.drawingBoard.frame = self.bounds
    }
    public required init(coder: NSCoder) {
        self.drawingBoard = DrawingBoardView(frame: CGRect())
        super.init(coder: coder)
        self.addSubview(self.drawingBoard)
        self.drawingBoard.frame = self.bounds
    }
}
