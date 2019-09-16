//
//  AppDelegate.swift
//  TestStruct
//
//  Created by vector on 2019/9/10.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa


class Layout {
    
    init() {
        
    }
    deinit {
        print("dealloc ")
    }
    
}

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
    
    
    fileprivate var items: NSPointerArray = NSPointerArray(options: NSPointerFunctions.Options.weakMemory)
    
    fileprivate func append(layout: Layout) {
        var objPtr = Unmanaged.passUnretained(self).toOpaque();
        let _ /*ptrptr*/ = UnsafeRawPointer(&objPtr)
        //        let value: UInt = ptrptr.load(as: UInt.self)
        
        
        self.items.addPointer(objPtr)
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
        
        self.append(layout: Layout())
        self.append(layout: Layout())

        for idx in 0 ..< self.items.count {
            if let p = self.items.pointer(at: idx) {
                print("p:\(p)")
                
                let obj = Unmanaged<Layout>.fromOpaque(p).takeRetainedValue()
                
                print("obj:\(obj)")
                
            } else {
                print("ppp")
            }
        }
        
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

