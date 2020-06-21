//
//  Base.swift
//  LiveTeaching
//
//  Created by vector on 2020/6/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public struct Point {
    public var x: Int32
    public var y: Int32
    
    public static let zero: Point = Point(x: 0, y: 0)
    
    
    public static let minChange: Point = {
        var p = Point(x: 1, y: 1)
        if UInt64(Int.max) == UInt64(Int64.max) {
            p.x = 2
        }
        return p
    }()    
}
public struct Size {
    public var width: Int32
    public var height: Int32
    
    public var area: Int {
        return Int(self.width) * Int(self.height)
    }
    
    public static let zero: Size = Size(width: 0, height: 0)
}
public struct Rect {
    public var origin: Point
    public var size: Size
    
    public init(origin: Point, size: Size) {
        self.origin = origin
        self.size = size
    }
    
    public func standardize() -> Rect {
        var result = self
        if result.size.width < 0 {
            result.origin.x += result.size.width
            result.size.width *= -1
        }
        if (result.size.height < 0) {
            result.origin.y += result.size.height
            result.size.height *= -1
        }
        return result
    }
    
    public static let zero: Rect = Rect(origin: Point.zero, size: Size.zero)

}

extension Point: Equatable {
    public static func == (_ lhs: Point, _ rhs: Point) -> Bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
}
extension Point: Hashable {
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.x)
        hasher.combine(self.y)
    }
}

extension Size {
    public var cgSize: CGSize {
        let scale = UIScreen.main.scale
        return CGSize(width: CGFloat(self.width) / scale, height: CGFloat(self.height) / scale)
    }
}
extension Size: Equatable {
    public static func == (_ lhs: Size, _ rhs: Size) -> Bool {
        return lhs.width == rhs.width && lhs.height == rhs.height
    }
}

extension Rect {
    public init(x: Int32, y: Int32, width: Int32, height: Int32) {
        self.size = Size(width: width, height: height)
        self.origin = Point(x: x, y: y)
    }
    public init(width: Int32, height: Int32) {
        self.size = Size(width: width, height: height)
        self.origin = Point.zero
    }
}
extension Rect: Equatable {
    public static func == (_ lhs: Rect, _ rhs: Rect) -> Bool {
        return lhs.origin == rhs.origin && lhs.size == rhs.size
    }
}
