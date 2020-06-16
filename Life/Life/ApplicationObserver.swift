//
//  ApplicationObserver.swift
//  Life
//
//  Created by vector on 2020/6/16.
//  Copyright © 2020 haoqi. All rights reserved.
//

import UIKit
import Darwin

public final class ApplicationObserver {
    public let id: String
    public let bootTime: Int64

    public let bytes: NSMutableData
    
    public init() {
        let minSize: size_t = 128
        let pageSize: size_t = size_t(PAGE_SIZE)
        var size: size_t = pageSize
        if size < minSize {
            size = (minSize / pageSize) * pageSize
            if minSize % pageSize != 0 {
                size += pageSize
            }
        }
        let uuid: UUID = UUID()
        let id = (uuid.uuidString as NSString).replacingOccurrences(of: "-", with: "").lowercased()
        let date = Date()
        func upTime() -> Int64 {
            let time = CACurrentMediaTime() * 1000000.0
            return Int64(time)
        }
        let bootTime = upTime()
        let bytes = NSMutableData(length: size)!
        self.bytes = bytes
        self.id = id
        self.bootTime = bootTime
        let data = "\(date)-\(bootTime)".data(using: .utf8)!
        
        var path = NSSearchPathForDirectoriesInDomains(.libraryDirectory, FileManager.SearchPathDomainMask.userDomainMask, true).first!
        if path.hasSuffix("/") {
            path += id
        } else {
            path += "/"
            path += id
        }
        path += "/runing"
//        try! Data(repeating: 0, count: size).write(to: URL(fileURLWithPath: path))
        open((path as NSString).utf8String!, O_RDWR, O_CREAT)
        
        let fd = open((path as NSString).utf8String!, O_RDWR, O_CREAT)
        assert(fd != -1)

        assert(size > 0);
        let ptr = mmap(nil, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)
        memcpy(ptr, <#T##__src: UnsafeRawPointer!##UnsafeRawPointer!#>, <#T##__n: Int##Int#>)

    }
    

    
}
