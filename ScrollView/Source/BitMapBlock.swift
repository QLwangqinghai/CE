//
//  BitMapBlock.swift
//  ScrollView
//
//  Created by vector on 2019/8/22.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import CoreGraphics.CGBitmapContext

public struct Rect {
    public var x: Int
    public var y: Int
    public var width: Int
    public var height: Int
}

public final class BitMapBlockPool: Hashable {
    public static func == (lhs: BitMapBlockPool, rhs: BitMapBlockPool) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
    
    public let maxCache: Int

    public let blockWidth: Int
    public let blockHeight: Int
    public let blockSize: Int
    private var caches: [UnsafeMutableRawPointer]
    private let lock: NSLock
    private var _config: BitMapBlockConfig?

    public func config() -> BitMapBlockConfig {
        if let v = self._config {
            return v
        } else {
            let v = BitMapBlockConfig(width: blockWidth, height: blockHeight, deallocate: { (ptr) in
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
    public init?(blockWidth: Int, blockHeight: Int, maxCache: Int) {
        if blockHeight > 2048 || blockHeight < 1 {
            return nil
        }
        if blockWidth > 2048 || blockWidth < 1 {
            return nil
        }
        self.caches = []
        self.lock = NSLock()
        self.maxCache = maxCache
        self.blockWidth = blockWidth
        self.blockHeight = blockHeight
        self.blockSize = blockWidth * blockHeight * 4
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
    
    
    public func makeBlock(x: Int, y: Int)-> BitMapBlock {
        let pointer: UnsafeMutableRawPointer = {
            let p : UnsafeMutableRawPointer
            if let v : UnsafeMutableRawPointer = self.cachedBlock() {
                p = v
            } else {
                p = UnsafeMutableRawPointer.allocate(byteCount: self.blockSize, alignment: 8)
            }
            
            return p
        } ()

        return BitMapBlock(x: x, y: y, pointer: pointer, config: self.config())
    }
    
    
}

public final class BitMapBlockConfig {
    public let width: Int
    public let height: Int
    public let deallocate: (_ pointer: UnsafeMutableRawPointer) -> Void
    
    fileprivate init(width: Int, height: Int, deallocate: @escaping (_ pointer: UnsafeMutableRawPointer) -> Void) {
        self.width = width
        self.height = height
        self.deallocate = deallocate
    }
}



public final class BitMapBlock: Hashable {
    public static func == (lhs: BitMapBlock, rhs: BitMapBlock) -> Bool {
        return lhs === rhs
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
    
    public let config: BitMapBlockConfig
    public let frame: Rect
    public let pointer: UnsafeMutableRawPointer

    fileprivate init(x: Int, y: Int, pointer: UnsafeMutableRawPointer, config: BitMapBlockConfig) {
        self.frame = Rect(x: x, y: y, width: config.width, height: config.height)
        self.pointer = pointer
        self.config = config
    }
    
    func withContex<Result>(_ body: (_ context: CGContext) throws -> Result) rethrows ->Result {
        let bitsPerComponent: Int = 8
        let pointer: UnsafeMutableRawPointer = UnsafeMutableRawPointer.allocate(byteCount: self.frame.width * self.frame.height * 4, alignment: 8)
        
        let context = CGContext(data: pointer, width: self.frame.width, height: self.frame.height, bitsPerComponent: bitsPerComponent, bytesPerRow: self.frame.width * bitsPerComponent, space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: CGImageAlphaInfo.first.rawValue | CGImageByteOrderInfo.order32Little.rawValue, releaseCallback: nil, releaseInfo: nil)!
        
        return try body(context)
    }
    
    
    func aa() {

        
//        cgbitmapconte
//        cgbitmapcontext
//        CGBitmapContext.cr
//        let a = CGBitmapContextCreateWithData
//
    }
    
}
