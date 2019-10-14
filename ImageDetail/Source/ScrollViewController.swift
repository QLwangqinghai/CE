//
//  ScrollViewController.swift
//  ImageDetail
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


open class ScrollViewController: UIViewController {
    
    override open func loadView() {
        self.view = UIScrollView(frame: UIScreen.main.bounds)
        self.view.backgroundColor = .red
    }
    
    override open func viewDidLoad() {
        super.viewDidLoad()

    }
    


}
