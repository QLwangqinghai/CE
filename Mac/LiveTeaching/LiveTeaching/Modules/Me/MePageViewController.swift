//
//  MinePageViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation

public class MePageViewController: BaseViewController {

    public let tapGestureRecognizer: UITapGestureRecognizer = UITapGestureRecognizer()

    
    public override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        self.hidesBottomBarWhenPushed = false
    }
    public required init?(coder: NSCoder) {
        super.init(coder: coder)
        self.hidesBottomBarWhenPushed = false
    }

    override public func viewDidLoad() {
        super.viewDidLoad()
        
        self.tapGestureRecognizer.addTarget(self, action: #selector(MePageViewController.handleTapped))
        self.view.addGestureRecognizer(self.tapGestureRecognizer)
    }

    
    @objc private func handleTapped(recognizer: UITapGestureRecognizer) {
        let controller = TestDrawViewController()
        self.present(controller, animated: true) {
            print("present finish")
        }
    }
    
}
