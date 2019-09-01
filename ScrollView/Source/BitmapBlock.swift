//
//  BitmapBlock.swift
//  ScrollView
//
//  Created by vector on 2019/8/22.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import CoreGraphics.CGBitmapContext

public struct MediaTimingBitmap {
    public typealias Rect = MediaTimingBitmapRect_s

    public final class BlockConfig {
        public let width: UInt32
        public let height: UInt32
        public let bitsPerComponent: Int = 8
        public let bytesPerRow: Int

        public let deallocate: (_ pointer: UnsafeMutableRawPointer) -> Void
        
        fileprivate init(width: UInt32, height: UInt32, deallocate: @escaping (_ pointer: UnsafeMutableRawPointer) -> Void) {
            self.width = width
            self.height = height
            self.deallocate = deallocate
            self.bytesPerRow = Int(width) * 4
        }
    }
    
    
    public final class BlockPool: Hashable {
        public static func == (lhs: BlockPool, rhs: BlockPool) -> Bool {
            return lhs === rhs
        }
        public func hash(into hasher: inout Hasher) {
            hasher.combine(Unmanaged.passUnretained(self).toOpaque())
        }
        
        public let maxCache: Int
        
        public let blockWidth: UInt32
        public let blockHeight: UInt32
        public let blockSize: Int
        private var caches: [UnsafeMutableRawPointer]
        private let lock: NSLock
        private var _config: BlockConfig?
        
        public func config() -> BlockConfig {
            if let v = self._config {
                return v
            } else {
                let v = BlockConfig(width: blockWidth, height: blockHeight, deallocate: { (ptr) in
                    self.lock.lock()
                    if self.maxCache > self.caches.count {
                        self.caches.append(ptr)
                    }
                    self.lock.unlock()
                })
                self._config = v
                return v
            }
        }
        
        
        // 1 <= blockWidth <= 2048, 1 <= blockHeight <= 2048
        public init?(maxCache: Int) {
            let blockHeight: UInt32 = 256
            let blockWidth: UInt32 = 256
            self.caches = []
            self.lock = NSLock()
            self.maxCache = maxCache
            self.blockWidth = blockWidth
            self.blockHeight = blockHeight
            self.blockSize = Int(blockWidth * blockHeight * 4)
        }
        
        private func cachedBlock() ->UnsafeMutableRawPointer? {
            let p : UnsafeMutableRawPointer?
            self.lock.lock()
            if self.caches.count > 0 {
                p = self.caches.remove(at: 0)
            } else {
                p = nil
            }
            self.lock.unlock()
            return p
        }
        public func makeBlock(x: Int, y: Int) {
             let a = addddd()
        
            
        }

        
        
//        public func makeBlock(x: Int, y: Int)-> Block {
//            let pointer: UnsafeMutableRawPointer = {
//                let p : UnsafeMutableRawPointer
//                if let v : UnsafeMutableRawPointer = self.cachedBlock() {
//                    p = v
//                } else {
//                    p = UnsafeMutableRawPointer.allocate(byteCount: self.blockSize, alignment: 8)
//                }
//
//                return p
//            } ()
//
//            return Block(x: x, y: y, pointer: pointer, config: self.config())
//        }
        
        
    }
    
    public final class Block: Hashable {
        public static func == (lhs: Block, rhs: Block) -> Bool {
            return lhs === rhs
        }
        public func hash(into hasher: inout Hasher) {
            hasher.combine(Unmanaged.passUnretained(self).toOpaque())
        }
        
        public let config: BlockConfig
        public let frame: Rect
        public let time: Int64
        
        public let pointer: UnsafeMutableRawPointer
        
        fileprivate init(x: UInt32, y: UInt32, time: Int64, pointer: UnsafeMutableRawPointer, config: BlockConfig) {
            self.frame = Rect(x: x, y: y, width: config.width, height: config.height)
            self.time = time
            self.pointer = pointer
            self.config = config
        }
        
//        func withContext<Result>(_ body: (_ context: CGContext) throws -> Result) rethrows ->Result {
//            let bitsPerComponent: Int = 8
//            let pointer: UnsafeMutableRawPointer = UnsafeMutableRawPointer.allocate(byteCount: Int(self.frame.width * self.frame.height * 4), alignment: 8)
//
//            let context = CGContext(data: pointer, width: Int(self.frame.width), height: Int(self.frame.height), bitsPerComponent: bitsPerComponent, bytesPerRow: Int(self.frame.width) * bitsPerComponent, space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: CGImageAlphaInfo.first.rawValue | CGImageByteOrderInfo.order32Little.rawValue, releaseCallback: nil, releaseInfo: nil)!
//
//            return try body(context)
//        }
//
//        public func overlap(rect: Rect) -> Rect? {
//            let frame = self.frame
//
//
//
//        }
//
//        public func loopPixel(in rect: Rect, body: (_ context: CGContext) throws -> Result)
        
        
        
        func aa() {
            
            
            //        cgBitmapconte
            //        cgBitmapcontext
            //        CGBitmapContext.cr
            //        let a = CGBitmapContextCreateWithData
            //
        }
        
    }
    
    

    
    
    
}



public final class MediaTimingBitmapImage {
    fileprivate final class BlockQueue {
        fileprivate var blocks: [MediaTimingBitmap.Block] = []
        
        
        fileprivate func block(at time: Int64) -> MediaTimingBitmap.Block? {
            return nil
        }
        
        
    }

    
    
    
    
}
