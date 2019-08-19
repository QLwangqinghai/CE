//
//  main.swift
//  Add
//
//  Created by vector on 2019/8/19.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

print("Hello, World!")

print(Date())
var s: Int = 0
for index in 0 ..< 0x80000000 {
    s += index;
}
print(Date())

print(s)
