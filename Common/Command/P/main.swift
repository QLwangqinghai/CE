

import Foundation

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
    
    
    class func executeShell2(cmd: String) -> (Int, Data) {
        let task = Process()
        let outpipe = Pipe()
        task.standardOutput = outpipe
        
        task.launchPath = "/bin/bash"
        
        task.arguments = ["-c", cmd]
        
        task.launch()
        task.waitUntilExit()
        
        let reader = outpipe.fileHandleForReading
        let data = reader.readDataToEndOfFile()
        return (Int(task.terminationStatus), data)
    }
    
}

let arguments = ProcessInfo().arguments

var who: Int?
var find = false
for item in arguments {
    
    if !find {
        if item == "--params" {
            find = true
        } else {
            continue
        }
    } else {
        if item == "0" {
            who = 0
        } else if item == "1" {
            who = 1
        } else if item == "2" {
            who = 2
        }
        else if item == "3" {
            who = 3
        }
    }
}
guard let action = who else {
    print("error 2")
    exit(-1)
}



for index in 0 ..< 5000 {
    let s = String(format: "%04lld", index + 1);
    let buildUploadResult = Execution.executeShell2(cmd: "./Users/wangqinghai/work/eos/code/eos/build/programs/cleos push action vagastes1111 test '[\"bc.test\(action)\",\" \(action).\(s) EOS\", \"\"]' -p vagastes1111")
    
    if let ss = String(data: buildUploadResult.1, encoding: String.Encoding.utf8) {
        print(ss)
    }
    guard buildUploadResult.0 == 0 else {
        print("error 1")
        exit(-1)
    }
    

}

let timer = DispatchSource.makeTimerSource()


print("end!  \(action)")
