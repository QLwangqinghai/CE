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



public class BaseDisplay {
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
    public init() {
    }
}

//
//public protocol HeadBodyFootDisplayable where Self: DisplayCollection {
//    var header: Display? {get}
//    var footer: Display? {get}
//    var body: Display? {get}
//}

public class TableDisplay: BaseDisplay {

}

public class TableItem: TableDisplay {
    fileprivate(set) unowned var parent: TableSection?
    public var header: Display?
    public var footer: Display?
    public var body: Display?
    
    fileprivate override init() {
        super.init()
    }
}
//public class DisplayCollection {
//    public var items: [DisplayElement] {
//        return []
//    }
//    public init() {
//    }
//}

public class TableSection: TableDisplay {
    public var header: Display?
    public fileprivate(set) var items: [TableItem]
    public var footer: Display?

    public override init() {
        self.items = []
        super.init()
    }
    
    @discardableResult public addItem
    
}

public class TableContext {

}
public class TableContent: TableDisplay {
    fileprivate var contentView: UIView?

    public var header: Display?
    public var footer: Display?
    public fileprivate(set) var sections: [TableSection]
    
    public override init() {
        self.sections = []
        super.init()
    }
    public func reloadData() {
        
    }
}

public enum ScrollDirection: UInt32 {
    case vertical
    case horizontal
}

public enum ScrollViewContentStyle {
    case table(ScrollDirection)
    
}

public class ScrollContentContext {
    public private(set) unowned var contentView: UIView?
    
    
}

public class ScrollContentElement {
    private(set) var context: ScrollContentContext?
    
    public private(set) var context: ScrollContentContext?
    public var contentView: UIView? {
        return self.context?.contentView
    }
    
    public let view: UIView?
    public private(set) var width: CGFloat
    public private(set) var height: CGFloat
    
    public private(set) var frame: CGRect
    
    public init(view: UIView?, size: CGSize) {
        self.view = view
    }
    
    //    public func willMoveToParent
    
    
}

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
    
    public dynamic var elementSize: CGSize
    public dynamic var header: Item?
    public dynamic var body: Item?
    public dynamic var footer: Item?
    
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


public class ScrollSection {
    public private(set) var context: ScrollContentContext?
    public let view: UIView?
    
    public var contentView: UIView? {
        return self.context?.contentView
    }
    
    public private(set) var width: CGFloat
    public private(set) var height: CGFloat
    
    public private(set) var frame: CGRect
    
    public init(view: UIView) {
        <#statements#>
    }
    - (void)appendCell:(SCScrollViewCell *)item;
    - (void)prependCell:(SCScrollViewCell *)item;
    - (void)insertCell:(SCScrollViewCell *)item atIndex:(NSInteger)index;
    - (BOOL)removeCell:(SCScrollViewCell *)item;
}


open class ScrollView: UIScrollView {
    
    /*
     // Only override draw() if you perform custom drawing.
     // An empty implementation adversely affects performance during animation.
     override func draw(_ rect: CGRect) {
     // Drawing code
     }
     */
    
}
