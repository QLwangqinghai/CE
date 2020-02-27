//
//  Execution.swift
//  Alamofire
//
//  Created by vector on 2020/2/27.
//

import Foundation

class Execution: NSObject {

}

#if os(iOS) || os(watchOS) || os(tvOS)

#else

class Execution {
    class func execute(path: String, arguments: [String]? = nil) -> (Int, Data) {
        let task = Process()
        let outpipe = Pipe()
        
        task.standardOutput = outpipe
        
        
        task.launchPath = path
        if let argu = arguments {
            task.arguments = argu
        }
        task.launch()
        task.waitUntilExit()
        
        let reader = outpipe.fileHandleForReading
        
        
        let data = reader.readDataToEndOfFile()
        return (Int(task.terminationStatus), data)

    }
    
    class func executeShell(cmd: String) -> Int {
        let task = Process()
        task.launchPath = "/bin/bash"
        
        task.arguments = ["-c", cmd]

        task.launch()
        task.waitUntilExit()
        
        
        return Int(task.terminationStatus)
    }
}

#endif
