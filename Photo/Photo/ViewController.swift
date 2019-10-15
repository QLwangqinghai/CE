//
//  ViewController.swift
//  Photo
//
//  Created by vector on 2019/10/14.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

class ViewController: UIViewController {
    var dataSource: AssetDataSource?
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.view.backgroundColor = .red
        
        dataSource = AssetDataSource()
    }
    
//    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        PHPhotoLibrary.requestAuthorization { (status) in
//            if status == .authorized {
//                AssetGroupDataSource.test()
//            }
//        }
//    }


}

