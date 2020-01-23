//
//  Controller.swift
//  ScrollView
//
//  Created by vector on 2020/1/15.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

@_exported import Basic

//public class SliceCollection {
//    private class Row {
//        public let slices: [Slice]
//        public init(slices: [Slice]) {
//            self.slices = slices
//        }
//    }
//    private let rows: [Row]
//
//    private let slicesSize: C2DSize
//    public init?(size: C2DSize, slicesSize: C2DSize) {
//        if slicesSize.width < 0 || slicesSize.height < 0 {
//            return nil
//        }
//        if size.width < 0 || size.height < 0 {
//            return nil
//        }
//        self.slicesSize = slicesSize
//
//        let numberOfSlicePerRow: Int32
//        if slicesSize.width == 1 {
//            numberOfSlicePerRow = size.width
//        } else {
//            numberOfSlicePerRow = (size.width + slicesSize.width - 1) / slicesSize.width
//        }
//        let rowCount: Int32
//        if slicesSize.height == 1 {
//            rowCount = size.height
//        } else {
//            rowCount = (size.height + slicesSize.height - 1) / slicesSize.height
//        }
//        var rows: [Row] = []
//        for rowIndex in 0 ..< rowCount {
//            let y = slicesSize.height * rowIndex
//            var slices: [Slice] = []
//            slices.reserveCapacity(Int(numberOfSlicePerRow))
//            for sliceIndex in 0 ..< numberOfSlicePerRow {
//                let frame = C2DRectMake(sliceIndex * slicesSize.width, y, slicesSize.width, slicesSize.height)
//                slices.append(Slice(frame: frame))
//            }
//            rows.append(Row(slices: slices))
//        }
//        self.rows = rows
//    }
//}

//public class SliceCollection {
//    private let rows: [Slice]
//
//    private let sliceHeight: Int32
//    public init?(sliceCount: Int32, sliceHeight: Int32) {
//        if sliceHeight <= 0 {
//            return nil
//        }
//        if sliceCount < 0 {
//            return nil
//        }
//
//        var slices: [Slice] = []
//        if sliceCount > 0 {
//            for index in 0 ..< sliceCount {
//                let y = sliceHeight * index
//                slices.append(Slice(y: y))
//            }
//        }
//        self.rows = slices
//    }
//}



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

public class DrawingController {
    public let drawingBoardController: DrawingBoardController
    
    public let identifier: String
    public let container: DrawingContainer
    public private(set) var sequence: UInt64 = 0

    private let items: [Slice]
    public let bounds: Rect
    public let sliceHeight: UInt32

    public private(set) var offset: Int32
    public private(set) var visibleSlices: [Slice]
    public let layer: CALayer

    public let bitmapLayout: BitmapLayout
    public init(identifier: String, bounds: Rect, offset: Int32, bitmapLayout: BitmapLayout, scale: CGFloat) {
        let layer: CALayer = CALayer()
        layer.frame = CGRect(x: 0, y: 0, width: CGFloat(bounds.size.width) / scale, height: CGFloat(bounds.size.height) / scale)
        layer.masksToBounds = true
        
        let container: DrawingContainer = DrawingContainer(frame: layer.frame)
        container.layer.addSublayer(layer)
        
        self.drawingBoardController = DrawingBoardController(identifier: identifier.appending("/drawing"), size: bounds.size, offset: offset, bitmapLayout: bitmapLayout, scale: scale, layer: layer)
        
        let sliceHeight: UInt32 = 256
        let sliceCount: UInt32 = (bounds.size.height + sliceHeight - 1) / sliceHeight
        var slices: [Slice] = []
        let sliceSize = Size(width: bounds.size.width, height: sliceHeight)
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = Int32(sliceHeight * index)
                slices.append(Slice(ownerIdentifier: identifier, y: y, size: sliceSize, bitmapLayout: bitmapLayout))
            }
        }
        self.items = slices
        self.sliceHeight = sliceHeight
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.bounds = bounds
        self.offset = offset
        self.visibleSlices = []
        self.layer = layer
        self.container = container
                
    }
    

    



    
    
}
