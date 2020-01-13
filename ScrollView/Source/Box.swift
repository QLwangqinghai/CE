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


public class Chunk: NSObject {
    public let backgroundColor: UIColor
    
    
    
}
public class PageContext: NSObject {
    public let backgroundColor: UIColor
    
    
    
}
public class Page: NSObject {
    public private(set) var context: PageContext?
    
    
}
