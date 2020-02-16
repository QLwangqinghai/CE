//
//  DrawingBoardCBridge.swift
//  ScrollView
//
//  Created by vector on 2020/1/13.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit
@_exported import CCFoundation

public typealias Rect = CCRect32
public typealias Size = CCSize32
public typealias Point = CCPoint32

public extension Size {
//    init(width: CCUInt32, height: CCUInt32) {
//        self.init()
//        self.width = width
//        self.height = height
//    }
}
extension Size: Equatable {
    public static func == (_ lhs: Size, _ rhs: Size) -> Bool {
        return lhs.width == rhs.width && lhs.height == rhs.height
    }
}

public extension Point {
//    init(x: CCSInt32, y: CCSInt32) {
//        self.init()
//        self.x = x
//        self.y = y
//    }
}
extension Point: Equatable {
    public static func == (_ lhs: Point, _ rhs: Point) -> Bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
}

extension Rect {
    public init(x: CCSInt32, y: CCSInt32, width: CCUInt32, height: CCUInt32) {
        self.init()
        self.size = CCSize32Make(width, height)
        self.origin = CCPoint32Make(x, y)
    }
    public init(width: CCUInt32, height: CCUInt32) {
        self.init()
        self.size = CCSize32Make(width, height)
        self.origin = CCPoint32Make(0, 0)
    }
}
extension Rect: Equatable {
    public static func == (_ lhs: Rect, _ rhs: Rect) -> Bool {
        return lhs.origin == rhs.origin && lhs.size == rhs.size
    }
}



public struct CBridge {


}
