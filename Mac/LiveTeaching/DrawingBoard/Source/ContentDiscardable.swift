//
//  ContentDiscardable.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/22.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public protocol ContentDiscardable: class {
    associatedtype ContentType
    
    func beginContentAccess() -> ContentType
    func endContentAccess()
    func discardContentIfPossible()
    func isContentDiscarded() -> Bool
}
