//
//  Base.swift
//  ConnectKitDemo
//
//  Created by CaiLianfeng on 16/8/28.
//  Copyright © 2016年 WangQinghai. All rights reserved.
//

import Foundation


public enum ListenSignal: UInt32 {
    case `continue`
    case cancel
}
public struct IMLog {
    static var logCount: UInt64 = 0
    static let dateFormatter: DateFormatter = {
        let formatter = DateFormatter()
        let zone = TimeZone.current
        formatter.timeZone = zone
        formatter.dateFormat = "HH:mm:ss SSS"
        return formatter
    }()
    
    public static func info(_ message: @autoclosure () -> Any, _ path: String = #file, _ function: String = #function, line: Int = #line) {
            let date = Date()
            let zone = TimeZone(abbreviation: "GMT+0800")
            
            let interval = zone!.secondsFromGMT(for: date)
            let localeDate = date.addingTimeInterval(Double(interval))
            IMLog.logCount += 1
            var msgStr: String
            if let fileName = path.components(separatedBy: "/").last {
                msgStr = "[IM \(IMLog.dateFormatter.string(from: localeDate)) \(NSString(format: "%04d", IMLog.logCount))] thread: \(threadName()) fileName:\(fileName) function: \(function) line: \(line)\nmsg:\(message())"
            }else {
                msgStr = "[IM \(IMLog.dateFormatter.string(from: localeDate)) \(NSString(format: "%04d", IMLog.logCount))] thread: \(threadName()) function: \(function) line: \(line)\nmsg:\(message())"
            }
            print(msgStr)
    }
    
    static func threadName() -> String {
        if Thread.isMainThread {
            return "Main"
        } else {
            if let threadName = Thread.current.name , !threadName.isEmpty {
                return threadName
            } else {
                return String(format: "%p", Thread.current)
            }
        }
    }
}
