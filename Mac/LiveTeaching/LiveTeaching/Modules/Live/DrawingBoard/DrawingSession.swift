//
//  DrawingSession.swift
//  LiveTeaching
//
//  Created by vector on 2020/6/22.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit


public class DrawingSession {
    public class Item {
        public private(set) var sequences: IndexSet
        init(sequences: IndexSet) {
            self.sequences = sequences
        }
    }
    public let id: String
    public private(set) var current: Int
    public private(set) var sequences: IndexSet
    public private(set) var items: [BitmapTile.IndexPath: Item]
    public init(id: String, sequences: IndexSet, items: [BitmapTile.IndexPath: Item], current: Int) {
        self.id = id
        self.sequences = sequences
        self.items = items
        self.current = current
    }
    public init(id: String) {
        self.id = id
        self.sequences = IndexSet()
        self.items = [:]
        self.current = 0
    }
    subscript(key: BitmapTile.IndexPath) -> Item? {
        set {
            self.items[key] = newValue
        }
        get {
            return self.items[key]
        }
    }
}
