//
//  StorageManager.swift
//  Alamofire
//
//  Created by vector on 2020/2/28.
//

import Foundation

//适用于 非隐私、静态文件存储


public final class StorageManager {
    public let storages: [FileStorage]
    
    private init() {
        self.storages = []
    }
    
    public func storage(for file: FileIdentifier) -> FileStorage {
        return self.storages[Int(file.dgst.0)]
    }    
    
    public let shared: StorageManager = StorageManager()
}

extension FileStorage {
    
    
    
    
    
    
}
