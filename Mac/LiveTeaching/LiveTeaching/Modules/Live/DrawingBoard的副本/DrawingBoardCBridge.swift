//
//  DrawingBoardCBridge.swift
//  ScrollView
//
//  Created by vector on 2020/1/13.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

public typealias Rect = CCRect32
public typealias Size = CCSize32
public typealias Point = CCPoint32


public extension Point {

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

public extension Point {
//    init(x: CCSInt32, y: CCSInt32) {
//        self.init()
//        self.x = x
//        self.y = y
//    }
}


extension Rect {
    public init(x: Int32, y: Int32, width: Int32, height: Int32) {
        self.init()
        self.size = CCSize32Make(width, height)
        self.origin = CCPoint32Make(x, y)
    }
    public init(width: Int32, height: Int32) {
        self.init()
        self.size = CCSize32Make(width, height)
        self.origin = CCPoint32Make(0, 0)
    }
    
    public func standardize() -> Rect {
        return CCRect32Standardize(self)
    }

}
extension Rect: Equatable {
    public static func == (_ lhs: Rect, _ rhs: Rect) -> Bool {
        return lhs.origin == rhs.origin && lhs.size == rhs.size
    }
}



public struct CBridge {


}
