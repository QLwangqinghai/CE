//
//  Controller.swift
//  ScrollView
//
//  Created by vector on 2020/1/15.
//  Copyright © 2020 angfung. All rights reserved.
//

import Foundation


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



public class DrawingBoardController {
    public struct BoxConfig {
        public let colorSpace: DrawingContext.ColorSpace
        public let backgroundColor: DrawingContext.Color
        public init(backgroundColor: DrawingContext.Color = DrawingContext.Color(little32Argb: 0x00_ff_ff_ff) , colorSpace: DrawingContext.ColorSpace = .little32Argb) {
            self.colorSpace = colorSpace
            self.backgroundColor = backgroundColor
        }
    }
    
    private let items: [Slice]
    public let frame: Rect
    public init(frame: Rect) {
        let v = C2DRectStandardize(frame)
        self.frame = v
        self.config = config

        let sliceHeight: Int32 = 256
        let sliceCount: Int32 = v.size.height / sliceHeight
        let numberOfRows: Int = Int
        
        if sliceHeight <= 0 {
            return nil
        }
        if sliceCount < 0 {
            return nil
        }
        
        var slices: [Slice] = []
        if sliceCount > 0 {
            for index in 0 ..< sliceCount {
                let y = sliceHeight * index
                slices.append(Slice(y: y, height: sliceHeight, status: .normal))
            }
        }
        self.items = slices
        
    }
    
}
