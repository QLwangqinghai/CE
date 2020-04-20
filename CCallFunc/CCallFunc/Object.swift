//
//  Object.swift
//  CCallFunc
//
//  Created by vector on 2020/4/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

import Foundation

open class A: NSObject {
    
    
    
    deinit {
        print("A deinit")
    }
    
}

open class B: A {
    
    
    
    deinit {
        print("B deinit")
    }
    
}

open class C: B {
    
    
    
    deinit {
        print("C deinit")
    }
    
}
