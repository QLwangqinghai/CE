//
//  ScrollView.swift
//  ScrollView
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

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

open class ScrollViewCell {
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
