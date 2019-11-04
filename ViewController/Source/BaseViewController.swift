//
//  BaseViewController.swift
//  ViewController
//
//  Created by vector on 2019/10/26.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public class Controler: NSObject {
    
    
    
}
public class ControlerA<Value>: Controler {
    var array: [Value] = []
    
    
}


class BaseViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        
        self.view.layoutMarginsGuide
        
    }

    override func close(child: UIViewController, onResult: BackResultClosure?) {
        
    }

}


//public enum
public typealias BackResultClosure = (_ result: Bool) -> Void

@objc public protocol UIViewControllerJump {
    
    func close(child: UIViewController, onResult: BackResultClosure?)
    
//    func back(animate: Bool, onResult: BackResultClosure?)
//    func back(child: UIViewController, animate: Bool, onResult: BackResultClosure?)
}


extension UIViewController: UIViewControllerJump {
    public func close(child: UIViewController, onResult: BackResultClosure?) {
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 10) {
            if let body = onResult {
                body(true)
            }
        }
    }

//    public func back(child: UIViewController, animate: Bool, onResult: (_ result: Bool) -> Void) {
//
//    }
//
//    public func back(animate: Bool, onResult: BackResultClosure?) {
//
//    }

    func aa() {
//        self.back(animate: true) { (result) in
//
//        }
//        self.navigationController?.popViewController(animated: <#T##Bool#>)
    }
    
}

