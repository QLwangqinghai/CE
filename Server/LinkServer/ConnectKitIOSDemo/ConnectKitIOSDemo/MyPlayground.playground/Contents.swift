//: Playground - noun: a place where people can play

import Cocoa


var aa: UInt64 = 1
aa = (aa << 62)
let str = String(format: "%llx", aa)
print("str: \(str)")


let int1: UInt64 = 1
aa |= (int1 << 62)
let str2 = String(format: "%llx", aa)
print("str2: \(str2)")