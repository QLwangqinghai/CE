//
//  DrawingBoard.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
@_exported import CCFoundation
@_exported import Basic

public class DrawingBoard {
    public struct DrawingBoardContext: Equatable {
        public var identifier: String
        public var sequence: UInt64
        public var offset: Int32
        public static func == (_ lhs: DrawingBoardContext, _ rhs: DrawingBoardContext) -> Bool {
            return lhs.identifier == rhs.identifier && lhs.sequence == rhs.sequence && lhs.offset == rhs.offset
        }
    }
    
    public private(set) var context: DrawingBoardContext? = nil
    public let size: Size
    
    public let bitmapBuffer: BitmapByteBuffer
    public let bitmapContext: CGContext
    public let bitmapInfo: BitmapInfo
    public init(size: Size, bitmapInfo: BitmapInfo) {
        assert(size.height > 0)
        assert(size.width > 0)
        let bytesPerRow = Int(bitmapInfo.bytesPerPixel * size.width)
        self.bitmapInfo = bitmapInfo
        let bitmapBuffer = BitmapByteBuffer(size: size, bitmapInfo: bitmapInfo)
        self.bitmapBuffer = bitmapBuffer

        let colorSpace = bitmapInfo.space
        self.bitmapContext = CGContext(data: bitmapBuffer.ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(bitmapInfo.bitsPerComponent), bytesPerRow: bytesPerRow, space: colorSpace, bitmapInfo: bitmapInfo.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.size = size
    }
    
    private func clear() {
        
    }
    
    public func map(rect: CGRect, scale: CGFloat) -> Rect? {
        var frame = rect.standardized
        frame = rect.inset(by: UIEdgeInsets(top: 2, left: 2, bottom: 2, right: 2))
        var bounds = Rect(x: Int32(frame.origin.x * scale), y: Int32(frame.origin.y * scale), width: Int32(frame.size.width * scale), height: Int32(frame.size.height * scale))

//        bounds = C2DRectEnlargeFrame(<#T##from: C2DRect##C2DRect#>, <#T##to: C2DRect##C2DRect#>)


        if Int64(bounds.origin.x) + Int64(bounds.size.width) < 0 {
            return nil
        }
        if Int64(bounds.origin.y) + Int64(bounds.size.height) < 0 {
            return nil
        }
        
        if bounds.origin.x < 0 {
            bounds.size.width -= Int32(rect.origin.x * -1)
            bounds.origin.x = 0
        }
        if rect.origin.y < 0 {
            bounds.size.height -= Int32(rect.origin.y * -1)
            bounds.origin.y = 0
        }
///TODO: F
        
//        let alignment = Int32(CCGetCachelineSize()) / self.bitmapInfo.colorSpace.bytesPerPixel
//
//        if alignment > 1 {
//            let xOffset = bounds.origin.x % Int32(alignment)
//            bounds.origin.x -= xOffset
//            bounds.size.width += Int32(xOffset)
//
//            let widthOffset = bounds.size.width % Int32(alignment)
//            if widthOffset > 0 {
//                bounds.size.width += (Int32(alignment) - widthOffset)
//            }
//        }
        return bounds
    }

    
//    public func map(rect: CGRect, config: DrawingContainer.BoxConfig, originY: Int32, frames: inout [Int32: C2DBytesRange]) -> C2DRect? {
//        var frame = rect.standardized
//        frame = rect.inset(by: UIEdgeInsets(top: 2, left: 2, bottom: 2, right: 2))
//        let scale = config.scale
//        var bounds = C2DRectMake(Int32(tmp.origin.x * scale) - 1, Int32(tmp.origin.y * scale) - 1, Int32(tmp.size.width * scale) + 2, Int32(tmp.size.height * scale) + 2)
//        let m = C2DRectMake(0, 0, config.contextSize.width, INT32_MAX)
//
//        if !C2DRectIntersect(m, bounds, &bounds) {
//            return nil
//        }
//
////        bounds = C2DRectEnlargeFrame(<#T##from: C2DRect##C2DRect#>, <#T##to: C2DRect##C2DRect#>)
//        if bounds.origin.x % 2 != 0 {
//            bounds.origin.x -= 1
//            bounds.size.width += 1
//        }
//        if bounds.origin.y % 2 != 0 {
//            bounds.origin.y -= 1
//            bounds.size.height += 1
//        }
//        if bounds.size.width % 2 != 0 {
//            bounds.size.width += 1
//        }
//        if bounds.size.height % 2 != 0 {
//            bounds.size.height += 1
//        }
//
//        var rect = C2DRect()
//        if rect.origin.x < 0 {
//            rect.size.width += rect.origin.x
//            rect.origin.x = 0
//        }
//        if rect.origin.y < 0 {
//            rect.origin.y = 0
//        }
//
//        if config.colorSpace.bytesPerPixel < 8 {
//            let mask = (8 / config.colorSpace.bytesPerPixel)
//            let offset = x % mask
//            if offset != 0 {
//                rect.origin.x -= offset
//                rect.size.width += offset
//            }
//        }
//        return rect
//    }

    
//    public func begin(controller: DrawingBoardController, offset: Int32) throws {
//        let context = DrawingBoardContext(identifier: controller.pageContext.identifier, sequence: controller.sequence, offset: offset)
//        if self.context != context {
//            self.context = context
//            self.clear()
//        }
//    }
    
    public func beginDrawing() {}
    
    public func finish() {
        
    }
}

