//
//  ScrollView.swift
//  ScrollView
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


public class Display {
    public init() {
    }
}

public struct DisplayIdentifier: Hashable {
    public let address: UInt
    public let mask: UInt
    
    private static var sequence: UInt = 0
    private static func nextSequence() -> UInt {
        sequence &+= 1
        return sequence
    }
    public init(_ obj: AnyObject) {
        var objPtr = Unmanaged.passUnretained(obj).toOpaque()
        let ptrptr = UnsafeRawPointer(&objPtr)
        let value: UInt = ptrptr.load(as: UInt.self)
        self.address = value
        self.mask = DisplayIdentifier.nextSequence()
    }
    
    public static func == (lhs: DisplayIdentifier, rhs: DisplayIdentifier) -> Bool {
        return lhs.address == rhs.address && lhs.mask == rhs.mask
    }
    public func hash(into hasher: inout Hasher) {
        hasher.combine(self.address)
    }
}

public class BaseDisplay: Hashable {
//    public private(set) unowned var parent: BaseDisplay?
//    public private(set) var childs: [BaseDisplay] = []
//
//    fileprivate func append(_ sub: BaseDisplay) -> Bool {
//        if let _ = sub.parent {
//            return false
//        }
//        self.childs.append(sub)
//        sub.parent = self
////        sub.didMoveToParent()
//        return true
//    }
//    fileprivate func remove(at index: Int) {
//        self.childs.remove(at: index)
//        childs[index]
//
//        if let _ = sub.parent {
//            return false
//        }
//        self.childs.append(sub)
//        sub.parent = self
//        sub.didMoveToParent()
//        return true
//    }
//
////    public func didMoveToParent() {
////    }
//
    private var _identifier: DisplayIdentifier? = nil
    public var identifier: DisplayIdentifier {
        if let value = self._identifier {
            return value
        } else {
            let value = DisplayIdentifier(self)
            _identifier = value
            return value
        }
    }
    
    public init() {
        let _ = self.identifier
    }
    
    public static func == (lhs: BaseDisplay, rhs: BaseDisplay) -> Bool {
        return lhs.identifier == rhs.identifier
    }
    public func hash(into hasher: inout Hasher) {
        self.identifier.hash(into: &hasher)
    }
}

public enum ScrollDirection: UInt32 {
    case vertical
    case horizontal
}

public enum TableDisplayAlign {
    case autoHeight(CGFloat)
    case autoWidth(CGFloat)
}

public class BaseTableDisplay: BaseDisplay {
    public fileprivate(set) var align: TableDisplayAlign
    public fileprivate(set) var inset: CGFloat
    
    public init(align: TableDisplayAlign, inset: CGFloat = 0) {
        self.align = align
        self.inset = inset
        super.init()
    }
}

public class TableDisplay: BaseTableDisplay {
    
}

public class TableDisplayGroup: BaseTableDisplay {

}

public class TableDisplayView: UIView {
    
    public override init(frame: CGRect) {
        super.init(frame: frame)
        self.clipsToBounds = true
    }
    
    public required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        self.clipsToBounds = true
    }
    
    
}

public struct DisplayGroupSequence {
    public let main: Int32
    public let sub: Int32

    public init(main: Int32, sub: Int32) {
        self.main = main
        self.sub = sub
    }
    
    public static func == (lhs: DisplayGroupSequence, rhs: DisplayGroupSequence) -> Bool {
        return lhs.main == rhs.main && lhs.sub == rhs.sub
    }
    public static func < (lhs: DisplayGroupSequence, rhs: DisplayGroupSequence) -> Bool {
        return lhs.main != rhs.main ? (lhs.main < rhs.main) : lhs.sub < rhs.sub
    }
    public static func > (lhs: DisplayGroupSequence, rhs: DisplayGroupSequence) -> Bool {
        return lhs.main != rhs.main ? (lhs.main > rhs.main) : lhs.sub > rhs.sub
    }
}

public struct LayoutUpdateFlag: OptionSet {
    public var rawValue: UInt32
    
    public init(rawValue: UInt32) {
        self.rawValue = rawValue
    }
    
    static let originX: LayoutUpdateFlag = LayoutUpdateFlag(rawValue: 1 << 0)
    static let originY: LayoutUpdateFlag = LayoutUpdateFlag(rawValue: 1 << 1)
    static let width: LayoutUpdateFlag = LayoutUpdateFlag(rawValue: 1 << 2)
    static let height: LayoutUpdateFlag = LayoutUpdateFlag(rawValue: 1 << 3)
    static let all: LayoutUpdateFlag = [.originX, .originY, .width, .height]
    static let none: LayoutUpdateFlag = []
}


public class TableInsetItem: TableDisplay {
    internal private(set) var frame: CGRect = CGRect()
    
    public private(set) var size: CGSize {
        didSet(oldValue) {
            if oldValue != self.size {
                self.didSizeChanged(from: oldValue, to: self.size)
            }
        }
    }

    internal var displaySequence: Int
    
    public override var align: TableDisplayAlign {
        didSet(oldValue) {
            self.resetSize()
        }
    }

    public override var inset: CGFloat {
        didSet(oldValue) {
            if oldValue != self.inset {
                self.resetInsetInParent()
            }
        }
    }
    fileprivate func resetInsetInParent() {
        self.insetInParent = self.inset
    }
    fileprivate func resetSize() {
        var size: CGSize = CGSize()
        switch self.align {
        case .autoWidth(let width):
            size.width = width
            size.height = self.insetInParent
        case .autoHeight(let height):
            size.height = height
            size.width = self.insetInParent
        }
        self.size = size
    }
    fileprivate var insetInParent: CGFloat {
        didSet(oldValue) {
            if oldValue != self.insetInParent {
                self.resetSize()
            }
        }
    }
    public init(align: TableDisplayAlign, sequence: Int, inset: CGFloat = 0) {
        self.insetInParent = inset
        self.displaySequence = sequence
        
        var size: CGSize = CGSize()
        switch align {
        case .autoWidth(let width):
            size.width = width
            size.height = inset
        case .autoHeight(let height):
            size.height = height
            size.width = inset
        }
        self.size = size
        
        super.init(align: align, inset: inset)
    }
    internal func didSizeChanged(from: CGSize, to: CGSize) {
    }
}

public class TableItem: TableInsetItem {
    fileprivate(set) unowned var parent: TableSection?
    public var header: Display?
    public var footer: Display?
    public var body: Display?
    
    fileprivate override init(align: TableDisplayAlign, sequence: Int, inset: CGFloat = 0) {
        super.init(align: align, sequence: sequence, inset: inset)
    }
    
    public func updateInset(_ inset: CGFloat) {
        self.inset = inset
    }
    
    internal override func didSizeChanged(from: CGSize, to: CGSize) {
        if let parent = self.parent {
            parent.didSizeChanged(of: self, from: from, to: to)
        }
    }
}
internal class DisplayOrderedCollection<T: TableInsetItem> {
    internal var items: [T] = []
    public init() {
//        collection.objectf
    }
    //    open subscript(idx: Int) -> Any { get }

    public func append(_ item: T) {
        self.items.append(item)
    }
    public func prepend(_ item: T) {
        self.items.insert(item, at: 0)
    }
    public func insert(_ item: T, at index: Int) {
        if index == 0 {
            self.prepend(item)
        } else if index >= self.items.count {
            self.append(item)
        } else {
            self.items.insert(item, at: index)
        }
    }
    
    public func remove(at index: Int) -> T? {
        if index >= 0 && index < self.items.count {
            let value: T = self.items[index]
            self.items.remove(at: index)
            return value
        } else {
            return nil
        }
    }
    
    public func remove(item: T) -> Bool {
        if let index = self.items.firstIndex(of: item) {
            self.items.remove(at: index)
            return true
        } else {
            return false
        }
    }

}

public class TableSection: TableInsetItem {

    public var header: Display?
    public var footer: Display?
    fileprivate var _items: DisplayOrderedCollection<TableItem>
    
    public var items: [TableItem] {
        return _items.items
    }
    
    
    fileprivate override init(align: TableDisplayAlign, sequence: Int, inset: CGFloat = 0) {
        self._items = DisplayOrderedCollection<TableItem>()
        super.init(align: align, sequence: sequence, inset: inset)
    }
    private func resetInset() {
        var inset: CGFloat = 0
        
        
        self.inset = inset
    }

//    @discardableResult public addItem
    internal func didSizeChanged(of item: TableItem, from: CGSize, to: CGSize) {
        
    }
    
    public func insertItem(index: Int, beforeInsert: (_ item: TableItem) -> Void, afterInsert: (_ item: TableItem) -> Void) {
        let item = TableItem(align: self.align, sequence: index, inset: inset)
        beforeInsert(item)
        
        afterInsert(item)
    }
}

public class TableContext {

}
public class TableContent: TableDisplay {
    fileprivate var contentView: UIView?

    public var minSize: CGSize = CGSize()
    public var header: Display?
    public var footer: Display?
    fileprivate var _sections: DisplayOrderedCollection<TableSection>

    public var sections: [TableSection] {
        return _sections.items
    }
    public init(align: TableDisplayAlign) {
        self._sections = DisplayOrderedCollection<TableSection>()
        super.init(align: align, inset: 0)
    }
    public func reloadData() {
        
    }
    
    //    @discardableResult public addItem
    internal func didSizeChanged(of section: TableSection, item: TableItem, from: CGSize, to: CGSize) {
        
    }
}



public enum ScrollViewContentStyle {
    case table(ScrollDirection)
    
}

public class ScrollContentContext {
    public private(set) unowned var contentView: UIView?
    
    
}

//public class ScrollContentElement {
//    private(set) var context: ScrollContentContext?
//
////    public private(set) var context: ScrollContentContext?
//    public var contentView: UIView? {
//        return self.context?.contentView
//    }
//
//    public let view: UIView?
//    public private(set) var width: CGFloat
//    public private(set) var height: CGFloat
//
//    public private(set) var frame: CGRect
//
//    public init(view: UIView?, size: CGSize) {
//        self.view = view
//    }
//
//    //    public func willMoveToParent
//
//
//}

open class ScrollViewItem {
    //    open class HeaderView: UIView {
    //        public override init(frame: CGRect) {
    //            super.init(frame: frame)
    //        }
    //
    //        required public init?(coder aDecoder: NSCoder) {
    //            super.init(coder: aDecoder)
    //        }
    //    }
    //    open class BodyView: UIView {
    //        public override init(frame: CGRect) {
    //            super.init(frame: frame)
    //        }
    //
    //        required public init?(coder aDecoder: NSCoder) {
    //            super.init(coder: aDecoder)
    //        }
    //    }
    //    open class FooterView: UIView {
    //        public override init(frame: CGRect) {
    //            super.init(frame: frame)
    //        }
    //
    //        required public init?(coder aDecoder: NSCoder) {
    //            super.init(coder: aDecoder)
    //        }
    //    }
    
    //    public private(set) var section: S
    
//    public dynamic var elementSize: CGSize
//    public dynamic var header: Item?
//    public dynamic var body: Item?
//    public dynamic var footer: Item?
    
    public init() {
    }
    
    
    /*
     // Only override draw() if you perform custom drawing.
     // An empty implementation adversely affects performance during animation.
     override func draw(_ rect: CGRect) {
     // Drawing code
     }
     */
    
}


//public class ScrollSection {
//    public private(set) var context: ScrollContentContext?
//    public let view: UIView?
//
//    public var contentView: UIView? {
//        return self.context?.contentView
//    }
//
//    public private(set) var width: CGFloat
//    public private(set) var height: CGFloat
//
//    public private(set) var frame: CGRect
//
//    public init(view: UIView) {
//        <#statements#>
//    }
//
//}


open class ScrollView: UIScrollView {
    
    /*
     // Only override draw() if you perform custom drawing.
     // An empty implementation adversely affects performance during animation.
     override func draw(_ rect: CGRect) {
     // Drawing code
     }
     */
    
}
