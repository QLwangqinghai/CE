//
//  Presentation.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/26.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
@_exported import CCFoundation



public class Presentation {
    public class DisplayItem {
        public let id: UInt64
        
        public init(id: UInt64) {
            self.id = id
        }
    }
    public let id: UInt64
    public let size: Size
    public let items: [DisplayItem] = []
    
    public init(id: UInt64, size: Size) {
        self.id = id
        self.size = size
    }
    
}
