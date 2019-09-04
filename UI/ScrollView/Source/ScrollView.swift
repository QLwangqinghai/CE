////
////  ScrollView.swift
////  ScrollView
////
////  Created by vector on 2019/9/3.
////  Copyright © 2019 angfung. All rights reserved.
////
//
//import UIKit
//
//
//public class Display {
//    public var subdisplays: [Display] {
//        return []
//    }
//
//    public init() {
//    }
//}
//
//
//
//public enum DisplayElement {
//    case display(Display)
//    case group(DisplayGroup)
//}
//public class DisplayGroup {
//    public var subitems: [DisplayCollection] {
//        return []
//    }
//    public init() {
//    }
//}
//public class DisplayCollection {
//    public enum ElementType {
//        case display
//        case group
//    }
//    public var elements: [DisplayElement] {
//        return []
//    }
//    public init() {
//    }
//}
//
////
////public protocol HeadBodyFootDisplayable where Self: DisplayCollection {
////    var header: Display? {get}
////    var footer: Display? {get}
////    var body: Display? {get}
////}
//
//public class TableItem: DisplayCollection {
////        public dynamic var elementSize: CGSize
//    public var header: Display?
//    public var footer: Display?
//    public var body: Display?
//
//    public override var elements: [DisplayElement] {
//        var items: [DisplayElement] = []
//        if let header = self.header {
//            items.append(DisplayElement.display(header))
//        }
//        if let body = self.body {
//            items.append(DisplayElement.display(body))
//        }
//        if let footer = self.footer {
//            items.append(DisplayElement.display(footer))
//        }
//        return items
//    }
//}
//public class TableGroup: DisplayCollection {
//
//    public override var elements: [DisplayElement] {
//        var items: [DisplayElement] = []
//        if let header = self.header {
//            items.append(DisplayElement.display(header))
//        }
//        if let body = self.body {
//            items.append(DisplayElement.display(body))
//        }
//        if let footer = self.footer {
//            items.append(DisplayElement.display(footer))
//        }
//        return items
//    }
//
//
//    public var items: [TableItem] {
//        return self.subdisplays as! [TableItem]
//    }
//}
//public class TableSection: Display, HeadBodyFootDisplayable {
//    public var header: Display?
//    public var footer: Display?
//    public let group: TableGroup
//    public var body: Display? {
//        return self.group
//    }
//
//    public override init() {
//        self.group = TableGroup()
//        super.init()
//    }
//
////        public dynamic var head: Group?
////        public dynamic var body: Group?
////        public dynamic var foot: Group?
//
//}
//
//
//
//public enum ScrollDirection: UInt32 {
//    case vertical
//    case horizontal
//}
//
//public enum ScrollViewContentStyle {
//    case table(ScrollDirection)
//
//}
//
//public class ScrollContentContext {
//    public private(set) unowned var contentView: UIView?
//
//
//}
//
//public class ScrollContentElement {
//    private(set) var context: ScrollContentContext?
//
//    public private(set) var context: ScrollContentContext?
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
////    public func willMoveToParent
//
//
//}
//
//open class ScrollViewItem {
////    open class HeaderView: UIView {
////        public override init(frame: CGRect) {
////            super.init(frame: frame)
////        }
////
////        required public init?(coder aDecoder: NSCoder) {
////            super.init(coder: aDecoder)
////        }
////    }
////    open class BodyView: UIView {
////        public override init(frame: CGRect) {
////            super.init(frame: frame)
////        }
////
////        required public init?(coder aDecoder: NSCoder) {
////            super.init(coder: aDecoder)
////        }
////    }
////    open class FooterView: UIView {
////        public override init(frame: CGRect) {
////            super.init(frame: frame)
////        }
////
////        required public init?(coder aDecoder: NSCoder) {
////            super.init(coder: aDecoder)
////        }
////    }
//
////    public private(set) var section: S
//
//    public dynamic var elementSize: CGSize
//    public dynamic var header: Item?
//    public dynamic var body: Item?
//    public dynamic var footer: Item?
//
//    public init() {
//    }
//
//
//    /*
//     // Only override draw() if you perform custom drawing.
//     // An empty implementation adversely affects performance during animation.
//     override func draw(_ rect: CGRect) {
//     // Drawing code
//     }
//     */
//
//}
//
//
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
//
//
//open class ScrollView: UIScrollView {
//
//    /*
//    // Only override draw() if you perform custom drawing.
//    // An empty implementation adversely affects performance during animation.
//    override func draw(_ rect: CGRect) {
//        // Drawing code
//    }
//    */
//
//}



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

public class TableDisplay: BaseDisplay {
    public private(set) var size: CGSize
    public var align: TableDisplayAlign
    public var offset: CGFloat

    public init(align: TableDisplayAlign, offset: CGFloat = 0) {
        self.align = align
        self.offset = offset
        
        var size: CGSize = CGSize()
        switch align {
        case .autoWidth(let width):
            size.width = width
            size.height = offset
        case .autoHeight(let height):
            size.height = height
            size.width = offset
        }
        self.size = size
        super.init()
    }
    
    
}

protocol TableDisplaySequence {
    var displaySequence: Int {
        get set
    }
}

public class TableItem: TableDisplay, TableDisplaySequence {
    var displaySequence: Int = 0
    fileprivate(set) unowned var parent: TableSection?
    public var header: Display?
    public var footer: Display?
    public var body: Display?
    
    
    
    fileprivate override init(align: TableDisplayAlign, offset: CGFloat = 0) {
        super.init(align: align, offset: offset)
    }
    
}
internal class DisplayOrderedCollection<T: TableDisplay & TableDisplaySequence> {
    public var items: [T] = []
    public init() {
    }
    
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

public class TableSection: TableDisplay, TableDisplaySequence {
    var displaySequence: Int = 0

    public var header: Display?
    public var footer: Display?
    fileprivate var _items: DisplayOrderedCollection<TableItem>
    
    public var items: [TableItem] {
        return _items.items
    }
    
    public override init() {
        self._items = DisplayOrderedCollection<TableItem>()
        super.init()
    }
    
//    @discardableResult public addItem
    
}

public class TableContext {

}
public class TableContent: TableDisplay {
    fileprivate var contentView: UIView?

    public var header: Display?
    public var footer: Display?
    fileprivate var _sections: DisplayOrderedCollection<TableSection>

    public var sections: [TableSection] {
        return _sections.items
    }
    
    public override init() {
        self._sections = DisplayOrderedCollection<TableSection>()
        super.init()
    }
    public func reloadData() {
        
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
