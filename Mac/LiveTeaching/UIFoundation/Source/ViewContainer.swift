//
//  ViewContainer.swift
//  Basic
//
//  Created by vector on 2020/2/16.
//

import UIKit

open class ViewContainer: StackView {    
    open override func point(inside point: CGPoint, with event: UIEvent?) -> Bool {
        guard self.isUserInteractionEnabled else {
            return false
        }
        guard self.alpha > 0.001 else {
            return false
        }
        guard !self.isHidden else {
            return false
        }
        guard self.bounds.contains(point) else {
            return false
        }
        
        for view in self.subviews {
            if view.point(inside: self.convert(point, to: view), with: event) {
                return true
            }
        }
        return false
    }

}
