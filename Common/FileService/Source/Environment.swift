//
//  Environment.swift
//  FileService
//
//  Created by vector on 2020/2/28.
//

import Foundation


/*
 {
    storages = [
        {
            lowerSize = UInt64
            upperSize = UInt64
            substorages = [
                {upper = UInt16, path = ""}
            ]
        },
    ]
 }
 
 
 
 */



public class Environment {
    public private(set) var config: [String: Any] = [:]
    
    private init() {
        let pro = ProcessInfo()
    }
    
    public let shared: Environment = Environment()
}


