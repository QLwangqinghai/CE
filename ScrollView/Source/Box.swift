//
//  Box.swift
//  ScrollView
//
//  Created by vector on 2020/1/13.
//  Copyright © 2020 angfung. All rights reserved.
//

import Foundation

public class BoxView: UIView {
    public override init(frame: CGRect) {
        super.init(frame: frame)
    }
    public required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
    
//    public init(config: DrawingContext.BoxConfig) {}

}




// MARK: - model

public class Box: NSObject {
    public let config: DrawingContext.BoxConfig
    public let pages: [Page]
    public init?(config: DrawingContext.BoxConfig, pages: [Page]) {
        guard config.scale >= 0.1, config.scale <= 10, !pages.isEmpty else {
            return nil
        }
        self.config = config
        self.pages = pages
        super.init()
    }
}

public class Page: NSObject {
    public private(set) var context: PageContext?
    
    public let identifer: Int = 0
    
}
public class Slice: NSObject {
    public init(y: Int32) {

    }
}

public class BoxContext: NSObject {
    public let backgroundColor: UIColor
    public init(backgroundColor: UIColor) {
        self.backgroundColor = backgroundColor
    }
}
public class PageContext: NSObject {
    public let backgroundColor: UIColor
    public init(backgroundColor: UIColor) {
        self.backgroundColor = backgroundColor
    }
}
public class SliceContext: NSObject {
    public init(frame: C2DRect) {

    }
}

