//
//  AssetBase.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

public struct ListChange<Element: Any> {
    public enum ChangeType: Int {
        case insert
        case remove
        case update
    }

    public let type: ChangeType
    public let items: [(Int, Element)]
}

public struct Order {
    public var main: UInt64
    public var originalOrder: UInt
    public static func < (lhs: Order, rhs: Order) -> Bool {
        return lhs.main == rhs.main ? lhs.originalOrder < rhs.originalOrder : lhs.main < rhs.main
    }
}

public final class ListObserver<ListEntity: AnyObject, Element: Any> {
    public typealias ReloadClosure = (_ entity: ListEntity) -> Void
    public typealias ChangeClosure = (_ entity: ListEntity, _ changes: [ListChange<Element>]) -> Void
    public typealias UpdateClosure = (_ entity: ListEntity, _ items: [(Int, Element)]) -> Void

    public let didReload: ReloadClosure
    public let didChange: ChangeClosure
    public let didUpdate: UpdateClosure

    public init(didReload: @escaping ReloadClosure, didChange: @escaping ChangeClosure, didUpdate: @escaping UpdateClosure) {
        self.didReload = didReload
        self.didChange = didChange
        self.didUpdate = didUpdate
    }
}
