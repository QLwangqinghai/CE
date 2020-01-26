//
//  MinePageViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation

public class MinePageViewController: BaseViewController {
    public override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
        self.hidesBottomBarWhenPushed = false
    }
    public required init?(coder: NSCoder) {
        super.init(coder: coder)
        self.hidesBottomBarWhenPushed = false
    }

}
