//
//  BaseAppDelegate.swift
//  UIFoundation
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

open class BaseAppDelegate: UIResponder, UIApplicationDelegate {

    open var window: UIWindow?

    public override init() {
        self.window = MainWindowManager.shared.window
        super.init()
    }
    
    
    open func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.
        
        if #available(iOS 13, *) {
            
            
        } else {
            let window = self.window!
            window.rootViewController = self.loadRootViewController()
            window.makeKeyAndVisible()
        }
        return true
    }

    open func loadRootViewController() -> UIViewController {
        return UIStoryboard(name: "Main", bundle: nil).instantiateInitialViewController()!
    }
    
    open func application(_ application: UIApplication, supportedInterfaceOrientationsFor window: UIWindow?) -> UIInterfaceOrientationMask {
        return .all
    }

    // MARK: UISceneSession Lifecycle
    @available(iOS 13.0, *)
    open func application(_ application: UIApplication, configurationForConnecting connectingSceneSession: UISceneSession, options: UIScene.ConnectionOptions) -> UISceneConfiguration {
        // Called when a new scene session is being created.
        // Use this method to select a configuration to create the new scene with.
        return UISceneConfiguration(name: "Default Configuration", sessionRole: connectingSceneSession.role)
    }

    @available(iOS 13.0, *)
    open func application(_ application: UIApplication, didDiscardSceneSessions sceneSessions: Set<UISceneSession>) {
        // Called when the user discards a scene session.
        // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
        // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
    }
}
