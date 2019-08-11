//
//  ViewController.swift
//  MemoryShare
//
//  Created by vector on 2019/8/10.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        CBridge.run()
        
        
        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

