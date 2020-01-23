//
//  Object.swift
//  Basic
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import Foundation

public protocol RefrenceObject {}

extension RefrenceObject where Self: AnyObject {
    
    public var addressDescription: String {
        return "\(Unmanaged.passUnretained(self).toOpaque())"
    }
}

extension NSObject: RefrenceObject {}
