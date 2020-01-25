//
//  BaseViewController.swift
//  UIBasic
//
//  Created by vector on 2020/1/4.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit



open class ControllerContentView: UIStackView {
    
}




open class BaseViewController: UIViewController {
    private static var BaseViewControllerObserveContext: Int = 0
    
    public private(set) var displayStatus: ViewDisplayStatus = .didDisappear
    
    public private(set) var navigationInteractivePopGestureRecognizer: UIGestureRecognizer?
    public let contentView: ControllerContentView = ControllerContentView()
    
    
    public override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        self.hidesBottomBarWhenPushed = true
    }
    
    required public init?(coder: NSCoder) {
        super.init(coder: coder)
        self.hidesBottomBarWhenPushed = true
    }
    
    
    deinit {
        if let recognizer = self.navigationInteractivePopGestureRecognizer {
            recognizer.addObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), options: [.new, .old], context: &BaseViewController.BaseViewControllerObserveContext)
        }
//        self.navigationInteractivePopGestureRecognizer = nil
    }
    
    private func _BaseViewController_updateNavigationInteractivePopGestureRecognizer(recognizer: UIGestureRecognizer?) {
        if let recognizer = self.navigationInteractivePopGestureRecognizer {
            recognizer.removeObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), context: &BaseViewController.BaseViewControllerObserveContext)
        }
        self.navigationInteractivePopGestureRecognizer = recognizer
        if let arecognizer = recognizer {
            arecognizer.addObserver(self, forKeyPath: #keyPath(UIGestureRecognizer.state), options: [.new, .old], context: &BaseViewController.BaseViewControllerObserveContext)
        }
    }
    
    override open func viewDidLoad() {
        super.viewDidLoad()
        
        let contentView = self.contentView
        contentView.frame = self.view.bounds
        self.view.addSubview(contentView)
        self.view.backgroundColor = UIColor.white
    }
    
    open override func willMove(toParent parent: UIViewController?) {
        print("\(self)willMove.b \(parent)")
        super.willMove(toParent: parent)
        print("\(self)willMove.e \(parent)")
    }
    
    open override func didMove(toParent parent: UIViewController?) {
        print("\(self)didMove.b \(parent)")
        super.didMove(toParent: parent)
        print("\(self)didMove.e \(parent)")
        if let controller = parent as? UINavigationController {
            self._BaseViewController_updateNavigationInteractivePopGestureRecognizer(recognizer: controller.interactivePopGestureRecognizer)
        } else {
            self._BaseViewController_updateNavigationInteractivePopGestureRecognizer(recognizer: nil)
        }
    }
    
    open override func viewWillAppear(_ animated: Bool) {
        self.displayStatus = .willApper
        print("\(self) viewWillAppear")
        super.viewWillAppear(animated)
    }

    open override func viewDidAppear(_ animated: Bool) {
        self.displayStatus = .didAppear

       print("\(self) viewDidAppear")
       super.viewDidAppear(animated)
    }

    open override func viewWillDisappear(_ animated: Bool) {
        self.displayStatus = .willDisapper

       print("\(self) viewWillDisappear")
       super.viewWillDisappear(animated)
    }

    open override func viewDidDisappear(_ animated: Bool) {
        self.displayStatus = .didDisappear

       print("\(self) viewDidDisappear")
       super.viewDidDisappear(animated)
    }
    
    open override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        if context == &BaseViewController.BaseViewControllerObserveContext {
//            print("keyPath:\(keyPath), object:\(object), change:\(change)")
//            if keyPath == #keyPath(UIGestureRecognizer.state) {
//
//
//            } else {
//
//            }
        } else {
            super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
        }
    }
    
    @objc open override func back(sender: UIViewController, animate: Bool, completion: (() -> Void)?) -> Bool {
        return super.back(sender: sender, animate: animate, completion: completion)
    }
}

open class BaseTestViewController: BaseViewController {

    public override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        self.hidesBottomBarWhenPushed = true
        self.title = "\(arc4random() % 1000)"
    }
    
    required public init?(coder: NSCoder) {
        super.init(coder: coder)
        self.hidesBottomBarWhenPushed = true
        self.title = "\(arc4random() % 1000)"
    }
    
    override open func viewDidLoad() {
        super.viewDidLoad()
        
        self.view.backgroundColor = UIColor(white: CGFloat(arc4random() % 1000) / 1000.0, alpha: 1.0)
        // Do any additional setup after loading the view.
    }
}
