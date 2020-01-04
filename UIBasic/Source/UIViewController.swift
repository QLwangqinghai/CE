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
    @objc func back(animate: Bool, sender: UIViewController, completion: @escaping ()-> Void) -> Bool {
        guard let parent = self.parent else {
            completion()
            return true
        }
        if parent == self.presentedViewController {
            parent.dismiss(animated: animate, completion: completion)
            return true
        }
        return false
    }
    
}



