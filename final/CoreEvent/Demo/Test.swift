//
//  Test.swift
//  Demo
//
//  Created by vector on 2019/4/18.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

import Cocoa
import Dispatch

class Test: NSObject {
    let source: CFRunLoopSource = CFRunLoopSourceCreate(nil, 0, UnsafeMutablePointer<CFRunLoopSourceContext>!)
    
    func aa() {
        DispatchQoS.userInteractive
        
        
        DispatchQueue.init(label: <#T##String#>, qos: DispatchQoS, attributes: <#T##DispatchQueue.Attributes#>, autoreleaseFrequency: <#T##DispatchQueue.AutoreleaseFrequency#>, target: <#T##DispatchQueue?#>)
        
        
    }
    
    
    
}
