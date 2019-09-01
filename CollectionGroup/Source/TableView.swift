//
//  TableView.swift
//  CollectionGroup
//
//  Created by vector on 2019/8/30.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public final class TableContext: NSObject {
    public dynamic var contentWidth: CGFloat = 0
    public dynamic var infoUpdateSequence: Int = 0

    private var info: [String : Any] = [:]
    
}


open class TableView: UITableView {
    public let context: TableContext = TableContext()
    open override var contentSize: CGSize {
        didSet(oldValue) {
            if oldValue.width != contentSize.width {
                context.contentWidth = oldValue.width;
            }
        }
    }
    
    public override init(frame: CGRect, style: UITableView.Style) {
        super.init(frame: frame, style: style)
        
        self.separatorStyle = .none
        self.estimatedRowHeight = 0
        self.estimatedSectionHeaderHeight = 0
        self.estimatedSectionFooterHeight = 0
        
        self.register(UITableViewCell.self)
        self.register(UITableViewHeaderFooterView.self, forHeaderFooterViewReuseIdentifier: NSStringFromClass(UITableViewHeaderFooterView.self))
        context.contentWidth = self.contentSize.width;
    }
    
    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        self.separatorStyle = .none
        self.estimatedRowHeight = 0
        self.estimatedSectionHeaderHeight = 0
        self.estimatedSectionFooterHeight = 0
        
        self.register(UITableViewCell.self)
        self.register(UITableViewHeaderFooterView.self, forHeaderFooterViewReuseIdentifier: NSStringFromClass(UITableViewHeaderFooterView.self))
        context.contentWidth = self.contentSize.width;
    }
    
    public final func register(_ cellClass: AnyClass) {
        self.register(cellClass, forCellReuseIdentifier: NSStringFromClass(cellClass))
    }

}
