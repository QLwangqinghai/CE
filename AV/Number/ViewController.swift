//
//  ViewController.swift
//  Number
//
//  Created by vector on 2019/7/25.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


extension UInt64 {
    @_specialize(exported: true, where T == ArraySlice<UInt8>)
    init<T: Collection>(bytes: T) where T.Element == UInt8, T.Index == Int {
        self = UInt64(bytes: bytes, fromIndex: bytes.startIndex)
    }
    
    @_specialize(exported: true, where T == ArraySlice<UInt8>)
    init<T: Collection>(bytes: T, fromIndex index: T.Index) where T.Element == UInt8, T.Index == Int {
        if bytes.isEmpty {
            self = 0
            return
        }
        
        let count = bytes.count
        
        let val0 = count > 0 ? UInt64(bytes[index.advanced(by: 0)]) << 56 : 0
        let val1 = count > 1 ? UInt64(bytes[index.advanced(by: 1)]) << 48 : 0
        let val2 = count > 2 ? UInt64(bytes[index.advanced(by: 2)]) << 40 : 0
        let val3 = count > 3 ? UInt64(bytes[index.advanced(by: 3)]) << 32 : 0
        let val4 = count > 4 ? UInt64(bytes[index.advanced(by: 4)]) << 24 : 0
        let val5 = count > 5 ? UInt64(bytes[index.advanced(by: 5)]) << 16 : 0
        let val6 = count > 6 ? UInt64(bytes[index.advanced(by: 6)]) << 8 : 0
        let val7 = count > 7 ? UInt64(bytes[index.advanced(by: 7)]) : 0
        
        self = val0 | val1 | val2 | val3 | val4 | val5 | val6 | val7
    }
}



class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        var bytes: [UInt8] = [0, 0, 0, 0, 0, 0, 0, 1]
        let a = UInt64(bytes: bytes, fromIndex: bytes.startIndex)
        print("a: \(a)")
    }


}

