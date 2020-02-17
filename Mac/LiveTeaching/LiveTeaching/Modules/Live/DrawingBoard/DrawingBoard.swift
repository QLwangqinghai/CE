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

//需要个正方形
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
    
    private let mirroring: UnsafeMutableRawPointer
    public let ptr: UnsafeMutableRawPointer
    public let bitmapContext: CGContext
    public let bitmapLayout: Drawing.BitmapLayout
    public init(size: Size, bitmapLayout: Drawing.BitmapLayout) {
        assert(size.height > 0)
        assert(size.width <= bitmapLayout.countPerRow)
        self.bitmapLayout = bitmapLayout
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
        self.ptr = ptr
        
        let mirroring = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(size.height), alignment: CCGetCachelineSize())
        self.mirroring = mirroring

        let colorSpace = bitmapLayout.colorSpace
        self.bitmapContext = CGContext(data: ptr, width: Int(size.width), height: Int(size.height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.size = size

        
    }
    
    deinit {
        self.ptr.deallocate()
    }
    
    private func clear() {
        
    }
    
    public func map(rect: CGRect, scale: CGFloat) -> Rect? {
        var frame = rect.standardized
        frame = rect.inset(by: UIEdgeInsets(top: 2, left: 2, bottom: 2, right: 2))
        var bounds = Rect(x: CCSInt32(frame.origin.x * scale), y: CCSInt32(frame.origin.y * scale), width: CCUInt32(frame.size.width * scale), height: CCUInt32(frame.size.height * scale))

//        bounds = C2DRectEnlargeFrame(<#T##from: C2DRect##C2DRect#>, <#T##to: C2DRect##C2DRect#>)


        if Int64(bounds.origin.x) + Int64(bounds.size.width) < 0 {
            return nil
        }
        if Int64(bounds.origin.y) + Int64(bounds.size.height) < 0 {
            return nil
        }
        
        if bounds.origin.x < 0 {
            bounds.size.width -= CCUInt32(rect.origin.x * -1)
            bounds.origin.x = 0
        }
        if rect.origin.y < 0 {
            bounds.size.height -= CCUInt32(rect.origin.y * -1)
            bounds.origin.y = 0
        }

        let alignment = UInt32(CCGetCachelineSize()) / self.bitmapLayout.colorSpace.bytesPerPixel
        
        if alignment > 1 {
            let xOffset = bounds.origin.x % Int32(alignment)
            bounds.origin.x -= xOffset
            bounds.size.width += UInt32(xOffset)

            let widthOffset = bounds.size.width % UInt32(alignment)
            if widthOffset > 0 {
                bounds.size.width += (UInt32(alignment) - widthOffset)
            }
        }
        return bounds
    }

    
//    public func map(rect: CGRect, config: DrawingContext.BoxConfig, originY: Int32, frames: inout [Int32: C2DBytesRange]) -> C2DRect? {
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

