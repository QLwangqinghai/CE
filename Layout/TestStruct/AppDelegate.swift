//
//  AppDelegate.swift
//  TestStruct
//
//  Created by vector on 2019/9/10.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!

    fileprivate struct ContextStorage {
        fileprivate var parent: Int?
        fileprivate var current: Int?
        fileprivate var child: Int? {
            return self.current ?? self.parent
        }
        
        fileprivate init(current: Int?) {
            self.current = current
            self.parent = nil
        }
    }
    
    fileprivate var s: ContextStorage = ContextStorage(current: 2) {
        willSet(new) {
            print("willSet")
            print(new)
        }
        didSet(old) {
            print("didSet")
            print(old)
        }
    }
    
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        s.parent = 1
        
        s.current = 2
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

