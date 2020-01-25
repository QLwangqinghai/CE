//
//  DrawingPageContext.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DrawingPageContext {
    public class Observer {
        public typealias OffsetObserveClosure = (_ observer: Observer, _ context: DrawingPageContext, _ from: Int32, _ to: Int32) -> Void
        public typealias IsHiddenObserveClosure = (_ observer: Observer, _ context: DrawingPageContext, _ to: Bool) -> Void

        public let identifier: String
        public var willUpdateOffset: OffsetObserveClosure
        public var didUpdateOffset: OffsetObserveClosure
        public var willUpdateIsHidden: IsHiddenObserveClosure
        public var didUpdateIsHidden: IsHiddenObserveClosure

        public init(identifier: String) {
            self.identifier = identifier
            self.willUpdateOffset = {(_, _, _, _) in
            }
            self.didUpdateOffset = {(_, _, _, _) in
            }
            self.willUpdateIsHidden = {(_, _, _) in
            }
            self.didUpdateIsHidden = {(_, _, _) in
            }
        }
    }

    
    public let ownerIdentifier: String
    public let domain: String
    public let identifier: String
    public let contentSize: Size
    public let zoomScale: CGFloat
    public let bounds: CGRect
    public private(set) var offset: Int32
    public let bitmapLayout: BitmapLayout
    public private(set) var isHidden: Bool

    private var observers: [Observer] = []
        
    public init(ownerIdentifier: String, domain: String, contentSize: Size, offset: Int32, bitmapLayout: BitmapLayout, zoomScale: CGFloat) {
        let identifier = ownerIdentifier.appending("/\(domain)")
        self.ownerIdentifier = ownerIdentifier
        self.domain = domain
        self.zoomScale = zoomScale
        self.bitmapLayout = bitmapLayout
        self.identifier = identifier
        self.contentSize = contentSize
        self.offset = offset
        self.bounds = CGRect(x: 0, y: 0, width: CGFloat(contentSize.width) / zoomScale, height: CGFloat(contentSize.height) / zoomScale)
        self.isHidden = true
    }
    
    public func updateOffset(_ offset: Int32) {
        if self.offset != offset {
        
            
            self.offset = offset
            
        }
    }
    
    public func updateIsHidden(_ isHidden: Bool) {
        if self.isHidden != isHidden {
        
            
            self.isHidden = isHidden
            
        }
    }
    
    
}
