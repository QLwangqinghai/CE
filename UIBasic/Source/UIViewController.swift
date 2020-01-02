//
//  UIViewController.swift
//  UIBasic
//
//  Created by vector on 2019/12/31.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

public class ViewControllerStatusHandler: NSObject {
    
    
    
}
public class ViewControllerMutableStatusHandler: ViewControllerStatusHandler {
    
    
    
}



private var _UIViewController_statusHandlerKey: Int = 0;

public extension UIViewController {

    @objc class func loadStatusHandler() -> ViewControllerStatusHandler {
        return ViewControllerStatusHandler()
    }
    
    var statusHandler: ViewControllerStatusHandler {
        let value = objc_getAssociatedObject(self, &_UIViewController_statusHandlerKey)
        if let v = value as? ViewControllerStatusHandler {
            return v
        } else {
            let handler: ViewControllerStatusHandler = self.classForCoder.loadStatusHandler()
            objc_setAssociatedObject(self, &_UIViewController_statusHandlerKey, handler, .OBJC_ASSOCIATION_RETAIN_NONATOMIC);
            return handler;
        }
    }
}


public class BaseViewController: UIViewController {
    
    public override func didMove(toParent parent: UIViewController?) {
        super.didMove(toParent: parent)
        
    }
    
    
}
