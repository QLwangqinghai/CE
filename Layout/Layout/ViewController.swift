//
//  ViewController.swift
//  Layout
//
//  Created by vector on 2019/9/8.
//  Copyright © 2019 vector. All rights reserved.
//

import UIKit

public class V: UIView {
    
}


class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
//        let _ = LayoutGroup(origin: LayoutVector.init(x: 0, y: 0), size: LayoutVector.init(x: 0, y: 0))
//        
//        let a = LayoutViewBridge.viewContentLayout(layout: ViewContentLayout.init(origin: LayoutVector.init(x: 0, y: 0), size: LayoutVector.init(x: 0, y: 0)))
//        a.bind(view: self.view)
        
//        self.view.window?
        
        let a: [Int] = [1, 2, 3]
        
        a.enumerated()
        LayoutManager.shared
        
    }


}

