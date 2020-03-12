//
//  ViewController.swift
//  iOSDemo
//
//  Created by vector on 2020/3/12.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let v = UIView(frame: CGRect.init(x: 120, y: 120, width: 200, height: 200))
        self.view.addSubview(v)
        
        
        v.layer.cornerRadius = 100
        v.layer.masksToBounds = true
    }


}

