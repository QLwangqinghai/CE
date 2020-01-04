//
//  BaseNavigationController.swift
//  UIBasic
//
//  Created by vector on 2020/1/4.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit

open class BaseNavigationController: UINavigationController {
    private static var BaseNavigationControllerObserveContext: Int = 0;

    deinit {
        if let recognizer = self.interactivePopGestureRecognizer {
            recognizer.addObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), options: [.new, .old], context: &BaseNavigationController.BaseNavigationControllerObserveContext)
        }
    }

        
        
    open override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        if context == &BaseNavigationController.BaseNavigationControllerObserveContext {
            print("keyPath:\(keyPath), object:\(object), change:\(change)")
            if keyPath == #keyPath(UIGestureRecognizer.state) {
                
                
            } else {
                
            }
        } else {
            super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
        }
    }
    
    public override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        if let recognizer = self.interactivePopGestureRecognizer {
            recognizer.addObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), options: [.new, .old], context: &BaseNavigationController.BaseNavigationControllerObserveContext)
        }
    }
    
    public override init(rootViewController: UIViewController) {
        super.init(rootViewController: rootViewController)
    }
    
    required public init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        if let recognizer = self.interactivePopGestureRecognizer {
            recognizer.addObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), options: [.new, .old], context: &BaseNavigationController.BaseNavigationControllerObserveContext)
        }
    }
    
    
    override open func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    open override func pushViewController(_ viewController: UIViewController, animated: Bool) {
        print("pushViewController.b")
        super.pushViewController(viewController, animated: animated)
        print("pushViewController.e")
    }
    open override func popViewController(animated: Bool) -> UIViewController? {
        print("popViewController.b")
        let result = super.popViewController(animated: animated)
        print("popViewController.e")
        return result
    }

    open override func popToRootViewController(animated: Bool) -> [UIViewController]? {
        print("popToRootViewController.b")
        let result = super.popToRootViewController(animated: animated)
        print("popToRootViewController.e")
        return result
    }

    open override func popToViewController(_ viewController: UIViewController, animated: Bool) -> [UIViewController]? {
        print("popToViewController.b %@", viewController)
        let result = super.popToViewController(viewController, animated: animated)
        print("popToViewController.e %@", viewController)
        return result
    }
    
    
    
    
}
