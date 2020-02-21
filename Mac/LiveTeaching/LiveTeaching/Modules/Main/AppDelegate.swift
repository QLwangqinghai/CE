//
//  AppDelegate.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation
import SwiftyBeaver

@UIApplicationMain
class AppDelegate: BaseAppDelegate {
    
    override init() {
        let file = FileDestination() // get new file destination instance
        // uses standard logging to swiftybeaver.log in the application cache directory
        file.format = "$DHH:mm:ss$d $C$L$c: $M"  // hour, minute, second, colored log level and message
        file.minLevel = .verbose
        SwiftyBeaver.addDestination(file)        
        SwiftyBeaver.addDestination(ConsoleDestination())
        print("init")
        super.init()
    }

    override func loadRootViewController() -> UIViewController {
        return MainTabBarViewController()
    }
    
    
}


