//
//  DrawingView.swift
//  ScrollView
//
//  Created by vector on 2019/9/2.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit



public class DrawingContext {
    public struct Config {
        public let blockSize: UInt32
        public let blockPerRow: UInt32
        public let numberOfRows: UInt32
        public let width: UInt32
        public let height: UInt32
        public let pixelsCount: UInt32
        public var colorSpace: ColorSpace = .little16Argb
        public var backgroundColor: Color = Color(little32Argb: 0)
        public let bufferSize: Int

        public init(width: UInt32, height: UInt32) {
            let blockSize: UInt32 = 256
            let blockPerRow = (width + blockSize - 1) / blockSize
            let numberOfRows = (height + blockSize - 1) / blockSize
            let w = blockPerRow * blockSize
            let h = numberOfRows * blockSize
            let pixelsCount = w * h
            let size: Int = Int(pixelsCount * colorSpace.bytesPerPixel)
            self.blockSize = blockSize
            self.blockPerRow = blockPerRow
            self.numberOfRows = numberOfRows
            self.width = w
            self.height = h
            self.pixelsCount = pixelsCount
            self.bufferSize = size
        }
    }
    

    public enum ColorSpace: UInt32 {
        case little16Argb = 0
        case little32Argb = 1
        
        public var bytesPerPixel: UInt32 {
            switch self {
            case .little16Argb:
                return 2
            case .little32Argb:
                return 4
            }
        }
    }
    public struct Color {
        public let color32: UInt32
        public let color16: UInt16
        public init(little32Argb: UInt32) {
            self.color32 = little32Argb
            let a = little32Argb & 0xf0_00_00_00
            let r = little32Argb & 0x00_f0_00_00
            let g = little32Argb & 0x00_00_f0_00
            let b = little32Argb & 0x00_00_00_f0
            self.color16 = UInt16(clamping: (a >> 16) + (r >> 12) + (g >> 8) + (b >> 4))
        }
        public init(little16Argb: UInt16) {
            self.color16 = little16Argb
            let c: UInt32 = UInt32(little16Argb)
            let a = c & 0x00_00_f0_00
            let r = c & 0x00_00_0f_00
            let g = c & 0x00_00_00_f0
            let b = c & 0x00_00_00_0f
            self.color32 = ((a << 16) + (r << 12) + (g << 8) + (b << 4))
        }
    }
    
    public let mainPtr: UnsafeMutableRawPointer
    public let duplicatePtr: UnsafeMutableRawPointer

    public let config: Config
    public init(config: Config) {
        self.mainPtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.duplicatePtr = UnsafeMutableRawPointer.allocate(byteCount: config.bufferSize, alignment: 128)
        self.config = config
    }
}




class DrawingView: UIView {

 
}
