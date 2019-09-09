//
//  ViewController.swift
//  TableView
//
//  Created by vector on 2019/9/6.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


public extension UIView {
    public static var map: [Int:String] = [:]
    
    
    public static func description(_ view: UIView?) -> String {
        if let aview = view {
            let objPtr = Unmanaged.passUnretained(aview).toOpaque()
            var str = "\(objPtr)"
            if let t = map[aview.tag] {
                str += "-\(t)"
            }
            return str
        } else {
            return "nil"
        }
    }
    
}


class View: UIView {
    public var t: NSString = ""
    
    override func layoutSubviews() {
        print("{")
        print("layoutSubviews \(self.t)")
        super.layoutSubviews()
        print("}")
    }
    
    override func addSubview(_ view: UIView) {
        print("{")
        print("addSubview:\(UIView.description(view)) \(self.t)")
        super.addSubview(view)
        print("}")
    }

    override func willRemoveSubview(_ subview: UIView) {
        print("{")
        print("willRemoveSubview:\(UIView.description(subview)) \(self.t)")
        super.willRemoveSubview(subview)
        print("}")
    }
    override func didAddSubview(_ subview: UIView) {
        print("{")
        print("didAddSubview:\(UIView.description(subview)) \(self.t)")
        super.didAddSubview(subview)
        print("}")
    }
    
    override func removeFromSuperview() {
        print("{")
        print("removeFromSuperview \(self.t)")
        super.removeFromSuperview()
        print("}")
    }
    
    open override func willMove(toSuperview newSuperview: UIView?) {
        print("{")
        print("willMove toSuperview:\(UIView.description(newSuperview)) \(self.t)")
        super.willMove(toSuperview: newSuperview)
        print("}")
    }
    
    open override func didMoveToSuperview() {
        print("{")
        print("didMoveToSuperview \(self.t)")
        super.didMoveToSuperview()
        print("}")
    }
    
    open override func willMove(toWindow newWindow: UIWindow?) {
        print("{")
        print("willMove toWindow:\(UIView.description(newWindow)) \(self.t)")
        super.willMove(toWindow: newWindow)
        print("}")
    }
    
    open override func didMoveToWindow() {
        print("{")
        print("[didMoveToWindow \(self.t)")
        super.didMoveToWindow()
        print("super: \(self.superview)")
        print("}")
        self.window
    }
}

class AViewController: UIViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.view.backgroundColor = UIColor .red
    }
    
    
}

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let view: View = View(frame: self.view.bounds)
        view.t = "view"
        self.view.addSubview(view)
        
        let view1: View = View(frame: self.view.bounds)
        view1.t = "viewA"
        view.addSubview(view1)

//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
//            self.navigationController?.pushViewController(AViewController(nibName: nil, bundle: nil), animated: true)
//        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            print("add ------------------")
            
            let view1: View = View(frame: self.view.bounds)
            view1.t = "view-B"
            view.addSubview(view1)
            print("remove =======------------------")

            view1.removeFromSuperview()
            print("------------------=========")

            return
            
            
            print("{")
            print("update frame y")
            var frame = view.frame
            frame.origin.y += 10
            view.frame = frame
            print("}")
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 4) {
            print("{")
            print("update frame x")
            var frame = view.frame
            frame.origin.x += 10
            view.frame = frame
            print("}")
        }
        

        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 6) {
            print("{")
            print("update frame width")
            var frame = view.frame
            frame.size.width -= 10
            view.frame = frame
            print("}")
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 5) {
            print("{")
            print("update frame height")
            var frame = view.frame
            frame.size.height -= 10
            view.frame = frame
            print("}")
        }
        
    }


}

