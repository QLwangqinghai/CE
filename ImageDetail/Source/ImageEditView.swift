//
//  ImageEditView.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

open class ImageEditView: UIView {
    public let scrollView: UIScrollView
    
    
    public override init(frame: CGRect) {
        self.scrollView = UIScrollView(frame: CGRect(origin: CGPoint(), size: frame.size))
        super.init(frame: frame)
    }
    
    required public init?(coder aDecoder: NSCoder) {
        self.scrollView = UIScrollView(frame: CGRect())
        super.init(coder: aDecoder)
    }
    
}
