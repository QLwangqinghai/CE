//
//  Box.swift
//  ScrollView
//
//  Created by vector on 2020/1/13.
//  Copyright © 2020 angfung. All rights reserved.
//

import Foundation

public class Box: UIView {
    public override init(frame: CGRect) {
        super.init(frame: frame)
    }
    public required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
}

public class Slice: NSObject {
    public let backgroundColor: UIColor
    public private(set) var backgroundImage: UIImage?
    public let fillColor: DrawingContext.Color
    public init(fillColor: DrawingContext.Color, backgroundColor: UIColor = UIColor.white, backgroundImage: UIImage? = nil) {
        self.fillColor = fillColor
        self.backgroundColor = backgroundColor
        self.backgroundImage = backgroundImage
    }
}

public class PageContext: NSObject {
    public let fillColor: DrawingContext.Color
    public init(fillColor: DrawingContext.Color) {
        self.fillColor = fillColor
    }
}

public class Page: NSObject {
    public private(set) var context: PageContext?
    
    
}
