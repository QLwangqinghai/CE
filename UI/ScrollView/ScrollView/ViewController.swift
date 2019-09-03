//
//  ViewController.swift
//  ScrollView
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit



public class ScrollSection11 {
    public func ppp() {
        var objPtr = Unmanaged.passUnretained(self).toOpaque()
        let ptrptr = UnsafeRawPointer(&objPtr)
        let value: uintptr_t = ptrptr.load(as: uintptr_t.self)
        print(objPtr)
        print(value)
        
        //        let a: intptr_t =
        
        //            print(addres:Unmanaged.passUnretained(self).toOpaque(), as: Int.self)
        
    }
    
    public init() {
        
    }
    
}


class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        ScrollSection11().ppp()
        
        
    }


}

