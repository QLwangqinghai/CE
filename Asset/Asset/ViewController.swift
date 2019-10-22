//
//  ViewController.swift
//  Asset
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

class vv: UIView {
    override init(frame: CGRect) {
        super.init(frame:frame)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}


class ViewController: UIViewController {
    var dataSource: AssetGroupDataProvider?
    
    let handler: GroupTableHandler = GroupTableHandler()
    
    var array: [Any] = []
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.view.backgroundColor = .red
        
//        dataSource = AssetGroupDataProvider()
        
        
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
        
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            PHPhotoLibrary.requestAuthorization { (status) in
                if status == .authorized {
                    self.dataSource = AssetGroupDataProvider()
                }
            }
        }
        self.view.addSubview(self.handler.tableView)
        self.handler.tableView.frame = self.view.bounds
        self.handler.update(dataProvider: AssetGroupDataProvider())

  
//        let test = MyTableHandler()
//        self.array.append(test)
//        self.view.addSubview(test.tableView)
//        test.tableView.frame = self.view.bounds
        
    }
    
//    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        PHPhotoLibrary.requestAuthorization { (status) in
//            if status == .authorized {
//                AssetGroupDataProvider.test()
//            }
//        }
//    }
    
    
}



