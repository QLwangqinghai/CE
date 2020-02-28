//
//  MainTabBarViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation

class MainTabBarViewController: UITabBarController {
    let homePage = HomePageViewController(nibName: nil, bundle: nil)
    let mePage = MePageViewController(nibName: nil, bundle: nil)

    override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        self.loadChildControllers()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        self.loadChildControllers()
    }
    
    private func loadChildControllers() {
        let homePageNavigation = BaseNavigationController(rootViewController: self.homePage)
        homePageNavigation.tabBarItem.title = "首页"
        
        let mePageNavigation = BaseNavigationController(rootViewController: self.mePage)
        mePageNavigation.tabBarItem.title = "我"
        
        self.addChild(homePageNavigation)
        self.addChild(mePageNavigation)
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
            Network.test()
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = Resource.Color.background
    }
    
    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .portrait
    }
    override var preferredInterfaceOrientationForPresentation: UIInterfaceOrientation {
        return .portrait
    }
    
}
