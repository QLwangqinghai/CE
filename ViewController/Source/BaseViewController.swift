//
//  BaseViewController.swift
//  ViewController
//
//  Created by vector on 2019/10/26.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class BaseViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        
        self.view.layoutMarginsGuide
        
    }


}


@objc public protocol UIViewControllerJump {
    func back(animate: Bool, onResult: (_ result: Bool) -> Void)
    func back(child: UIViewController, animate: Bool, onResult: (_ result: Bool) -> Void)
}


extension UIViewController: UIViewControllerJump {
    public func back(child: UIViewController, animate: Bool, onResult: (_ result: Bool) -> Void) {
        
    }

    public func back(animate: Bool, onResult: (_ result: Bool) -> Void) {

    }

    func aa() {
        self.back(animate: true) { (result) in
            
        }
        self.navigationController?.popViewController(animated: <#T##Bool#>)
    }
    
}

