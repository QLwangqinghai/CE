//
//  ViewController.swift
//  Asset
//
//  Created by vector on 2019/10/16.
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
        
//        dataSource = AssetDataSource()
        
        
        let label = UILabel()
        self.view.addSubview(label)
        
        let size0 = label.intrinsicContentSize
        label.text = "123141\n123"
        let size1 = label.intrinsicContentSize
        
        label.sizeToFit()
        
        let size2 = label.intrinsicContentSize

        print("\(size0)  \(size1)  \(size2)")
        
        
        let imageView: UIImageView = UIImageView()
        var image = UIImage(named: "edit_button_delete")
        
        print("\(imageView.intrinsicContentSize)")
        imageView.image = image
        print("\(imageView.intrinsicContentSize)")

        image = nil
        image = UIImage(named: "edit_button_delete.png")
        imageView.image = image
        print("\(imageView.intrinsicContentSize)")

        
        let button = UIButton(type: UIButton.ButtonType.custom)
        button.backgroundColor = .red
        self.view.addSubview(button)
        button.setTitle("ajklfjsa", for: .normal)
        
        button.frame = CGRect.init(x: 100, y: 100, width: 40, height: 10)
        
    }
    
//    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        PHPhotoLibrary.requestAuthorization { (status) in
//            if status == .authorized {
//                AssetDataSource.test()
//            }
//        }
//    }
    
    
}



