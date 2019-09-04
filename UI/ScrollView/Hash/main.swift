//
//  main.swift
//  Hash
//
//  Created by vector on 2019/9/4.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation


public struct Point {
    public let x: Int
    public let y: Int
    public let xy: Int
    public let l: Double
    public let description: Description

    public struct Description: CustomStringConvertible {
        public let x: String
        public let y: String
        public let xy: String
        public let xxyy: String

        public let l: String
        
        public let description: String
        
        public init(x: String, y: String, xy: String, xxyy: String, l: String) {
            self.x = x
            self.y = y
            self.l = l
            self.xy = xy
            self.xxyy = xxyy
            self.description = "[(\(x),\(y)), xy:\(xy), l:\(l)]"
        }
    }

    public init(x: Int, y: Int) {
        let xy = x + y
        let xxyy = x * x + y * y
        let l = sqrt(Double(xxyy))
        
        func descriptionInt(_ number: Int, align: Int = 4) -> String {
            var description: String = "\(number)"
            var paddingCount = align - description.count
            while paddingCount > 0 {
                description = " " + description
                paddingCount -= 1
            }
            return description
        }
        func descriptionDouble(_ number: Double, decimal: Int = 3, align: Int = 8) -> String {
            let format: String = "%0\(decimal)lf"
            var description: String = String(format: format, number)
            var paddingCount = align - description.count
            while paddingCount > 0 {
                description = " " + description
                paddingCount -= 1
            }
            return description
        }
        self.x = x
        self.y = y
        self.l = l
        self.xy = xy
        self.description = Description(x: descriptionInt(x, align:2), y: descriptionInt(y, align:2), xy: descriptionInt(xy, align:2), xxyy: descriptionInt(xxyy, align:4), l: descriptionDouble(l, decimal: 2, align: 5))
    }
}

var table: [[Point]] = []
for x in 0 ..< 10 {
    var row: [Point] = []
    for y in 0 ..< 10 {
        row.append(Point(x: x, y: y))
    }
    table.append(row)
}

print("xy\n")

for row in table {
    let rowString = row.map { (item) -> String in
        return item.description.xy
    }.joined(separator: " ")
    print(rowString)
}

print("xxyy\n")

for row in table {
    let rowString = row.map { (item) -> String in
        return item.description.xxyy
        }.joined(separator: " ")
    print(rowString)
}

print("l\n")

for row in table {
    let rowString = row.map { (item) -> String in
        return item.description.l
        }.joined(separator: " ")
    print(rowString)
}
