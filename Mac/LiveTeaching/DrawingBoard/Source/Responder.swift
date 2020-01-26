//
//  Responder.swift
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

open class Responder: Hashable {
    public static func == (lhs: Responder, rhs: Responder) -> Bool {
        return lhs === rhs
    }

    public func hash(into hasher: inout Hasher) {
        hasher.combine(Unmanaged.passUnretained(self).toOpaque())
    }
}
