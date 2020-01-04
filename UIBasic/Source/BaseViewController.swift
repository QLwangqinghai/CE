//
//  BaseViewController.swift
//  UIBasic
//
//  Created by vector on 2020/1/4.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

open class BaseViewController: UIViewController {
    
    override open func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }
    
    public override func willMove(toParent parent: UIViewController?) {
        super.willMove(toParent: parent)
        print("willMove")
    }
    
    
    
    public override func didMove(toParent parent: UIViewController?) {
        super.didMove(toParent: parent)
        print("didMove")
    }
    
    @objc public func back(animate: Bool, sender: UIViewController, onFinished:@escaping ()-> Void) {
        DispatchQueue.main.async {
            
        }
    }
    
}
