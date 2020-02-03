//
//  ViewController.swift
//  LinkServer
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        OBBridge.test2()

        OBBridge.test()
        let time = DispatchSource.makeTimerSource()
        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    

}

