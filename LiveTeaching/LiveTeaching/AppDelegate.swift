//
//  AppDelegate.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation

@UIApplicationMain
class AppDelegate: BaseAppDelegate {
    
    override init() {
        print("init")
        super.init()
    }

    override func loadRootViewController() -> UIViewController {
        return MainTabBarViewController()
    }
    
    
}

