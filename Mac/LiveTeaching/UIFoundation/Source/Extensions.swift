//
//  Extensions.swift
//  UIFoundation
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

extension UIWindow {
    public static let main: UIWindow = UIWindow(frame: UIScreen.main.bounds)
}

extension UIScreen {
    public var size: CGSize {
        return self.bounds.size
    }
}
