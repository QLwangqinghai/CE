//
//  Environment.swift
//  FileService
//
//  Created by vector on 2020/2/28.
//

import Foundation


/*
 {
 
    storageHomeDirecory = "绝对路径"
    storages = [
        {upper = UInt16, path = ""}

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


