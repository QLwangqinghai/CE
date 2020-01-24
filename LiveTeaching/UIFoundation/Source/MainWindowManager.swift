//
//  UIApplicationManager.swift
//  UIFoundation
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

@_exported import UIKit
@_exported import Basic


public final class MainWindowManager: NSObject {
    public var window: UIWindow {
        return UIWindow.main
    }
    
    private override init() {

        super.init()
    }
    
    public static let shared: MainWindowManager = MainWindowManager()
}
