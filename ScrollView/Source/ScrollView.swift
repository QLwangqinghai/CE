//
//  ScrollView.swift
//  ScrollView
//
//  Created by vector on 2019/8/19.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public final class ScrollView11: View {
    
}

public enum Origin : UInt16 {
    case mix = 0x1
    case zero = 0x0
    case max = 0x2
}

public extension ScrollView {
    fileprivate final class ContentLayout {
        fileprivate var bounds: Rect64
        
        fileprivate init(bounds: Rect64) {
            self.bounds = bounds
        }
    }
}

open class ScrollView: View {
    public enum ContentMode : UInt32 {// x y
        case center = 0x0
        case top = 0x001
        case bottom = 0x002
        case left = 0x100
        case right = 0x200
        case topLeft = 0x101
        case topRight = 0x201
        case bottomLeft = 0x102
        case bottomRight = 0x202
        
        fileprivate var origin: Point64 {
            switch self {
            case .center:
                return Point64(x: 0, y: 0)
            case .top:
                return Point64(x: 0, y: Int64(Int32.min))
            case .bottom:
                return Point64(x: 0, y: Int64(Int32.max))
            case .left:
                return Point64(x: Int64(Int32.min), y: 0)
            case .right:
                return Point64(x: Int64(Int32.max), y: 0)
            case .topLeft:
                return Point64(x: Int64(Int32.min), y: Int64(Int32.min))
            case .topRight:
                return Point64(x: Int64(Int32.max), y: Int64(Int32.min))
            case .bottomLeft:
                return Point64(x: Int64(Int32.min), y: Int64(Int32.max))
            case .bottomRight:
                return Point64(x: Int64(Int32.max), y: Int64(Int32.max))
            }
        }
    }
    
    fileprivate let contentLayout: ContentLayout
    fileprivate let contentMode: ContentMode

    public init(frame: Rect, contentMode: ContentMode = .top) {
        self.contentMode = contentMode
        var bounds: Rect64 = Rect64()
        bounds.origin = contentMode.origin
        self.contentLayout = ContentLayout(bounds:bounds)
        super.init(frame: frame)
    }
    
    

}