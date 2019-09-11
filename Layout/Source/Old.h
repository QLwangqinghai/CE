//
//  Old.h
//  Layout
//
//  Created by vector on 2019/9/9.
//  Copyright © 2019 vector. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Old : NSObject

@end

NS_ASSUME_NONNULL_END

/*
 
 
 public struct LayoutPoint: Hashable {
 public var x: Int32
 public var y: Int32
 public init(x: Int32, y: Int32) {
 self.x = x
 self.y = y
 }
 public static func == (lhs: LayoutPoint, rhs: LayoutPoint) -> Bool {
 return lhs.x == rhs.x && lhs.y == rhs.y
 }
 public func hash(into hasher: inout Hasher) {
 hasher.combine(self.x)
 hasher.combine(self.y)
 }
 }
 public struct LayoutSize: Hashable {
 public var width: Int32
 public var height: Int32
 public init(width: Int32, height: Int32) {
 self.width = width
 self.height = height
 }
 public static func == (lhs: LayoutSize, rhs: LayoutSize) -> Bool {
 return lhs.width == rhs.width && lhs.height == rhs.height
 }
 public func hash(into hasher: inout Hasher) {
 hasher.combine(self.width)
 hasher.combine(self.height)
 }
 }

 public struct LayoutRect: Hashable {
 public internal(set) var origin: LayoutPoint
 public internal(set) var size: LayoutSize
 public static func == (lhs: LayoutRect, rhs: LayoutRect) -> Bool {
 return lhs.origin == rhs.origin && lhs.size == rhs.size
 }
 public func hash(into hasher: inout Hasher) {
 hasher.combine(origin)
 }
 }

 
 public struct LayoutRect: Hashable {
 public var x: Int
 public var y: Int
 public var height: Int
 public var width: Int
 public init() {
 }
 public static func == (lhs: LayoutRect, rhs: LayoutRect) -> Bool {
 return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height
 }
 public func hash(into hasher: inout Hasher) {
 hasher.combine(x)
 hasher.combine(y)
 }
 }

 
 */
