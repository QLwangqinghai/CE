//
//  UIViewController.swift
//  UIBasic
//
//  Created by vector on 2019/12/31.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public enum ViewDisplayStatus {
    case willDisapper
    case didDisappear
    case willApper
    case didAppear
}


public class StatusHandler: NSObject {    
    public override init() {
        super.init()
    }

}

private var _UIViewController_statusHandlerKey: Int = 0;

public extension UIViewController {

    @objc class func makeStatusHandler() -> StatusHandler {
        return StatusHandler()
    }
    
    var statusHandler: StatusHandler {
        let value = objc_getAssociatedObject(self, &_UIViewController_statusHandlerKey)
        if let v = value as? StatusHandler {
            return v
        } else {
            let handler: StatusHandler = self.classForCoder.makeStatusHandler()
            objc_setAssociatedObject(self, &_UIViewController_statusHandlerKey, handler, .OBJC_ASSOCIATION_RETAIN_NONATOMIC);
            return handler;
        }
    }
    @objc func back(sender: UIViewController, animate: Bool, completion: (() -> Void)?) -> Bool {
        guard let parent = self.parent else {
            if let closure = completion {
                closure()
            }
            return true
        }
        if parent == self.presentedViewController {
            parent.dismiss(animated: animate, completion: completion)
            return true
        }
        return parent.close(child: self, sender:sender, animate: animate, completion: completion)
    }
    
    @discardableResult @objc func back() -> Bool {
        return self.back(sender: self, animate: true) {
            
        }
    }
    
    @objc func close(child: UIViewController, sender: UIViewController, animate: Bool, completion: (() -> Void)?) -> Bool {
        return false
    }
    
    
}

public extension UINavigationController {
    @objc override func close(child: UIViewController, sender: UIViewController, animate: Bool, completion: (() -> Void)?) -> Bool {
        if super.close(child: child, sender: sender, animate: animate, completion: completion) {
            return true
        }
        
        if let index = self.viewControllers.firstIndex(of: child) {
            if index == 0 {
                return self.back(sender: sender, animate: animate, completion: completion)
            } else {
                self.popToViewController(self.viewControllers[index - 1], animated: animate)
                if let closure = completion {
                    DispatchQueue.main.async(execute: closure)
                }
                return true
            }
        } else {
            return false
        }
    }
    
    
}



