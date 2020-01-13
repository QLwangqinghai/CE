//
//  ViewController.swift
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        UIColor.init(patternImage: UIImage)
        
        
        
        let mem = TMemory()
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 5) {
            mem.test()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 8) {
            mem.test()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 11) {
            mem.test()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 14) {
            mem.test()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 17) {
            mem.test()
        }
    }


}

