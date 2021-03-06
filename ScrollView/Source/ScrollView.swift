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
        var _origin: Point64
        var _size: Size64
        
        fileprivate var origin: Point64 {
            set {
                _origin = newValue
            }
            get {
                return _origin
            }
        }
        fileprivate var size: Size64 {
            set {
                _size = newValue
            }
            get {
                return _size
            }
        }
        
        fileprivate init(origin: Point64, size: Size64) {
            _origin = origin
            _size = size
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
    public init(origin: Point, size: Size, contentMode: ContentMode = .top) {
        self.contentMode = contentMode
        self.contentLayout = ContentLayout(origin: contentMode.origin, size: Size64())
        super.init(origin: origin, size: size)
    }
    
    

}

open class CollectionReusableView: View {
    public struct ReuseIdentifier: Hashable {
        public let name: String
        public let style: String
        public static func == (lhs: ReuseIdentifier, rhs: ReuseIdentifier) -> Bool {
            return lhs.name == rhs.name && lhs.style == rhs.style
        }
        public func hash(into hasher: inout Hasher) {
            hasher.combine(self.name)
            hasher.combine(self.style)
        }
    }
    public let reuseIdentifier: ReuseIdentifier

    internal init(size: Size, reuseIdentifier: ReuseIdentifier) {
        self.reuseIdentifier = reuseIdentifier
        super.init(origin: Point(), size: size)
        self.onCreate()
    }
//    - (void)prepareForReuse;
    
    
    open func onCreate() {}
    
    
    
    
    
}



/*
 update x
 update y
 
 
 */
