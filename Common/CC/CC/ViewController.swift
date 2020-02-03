//
//  ViewController.swift
//  CC
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        OCBridge.testAes()
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

