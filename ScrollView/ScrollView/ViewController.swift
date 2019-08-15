//
//  ViewController.swift
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    let timer: DispatchSourceTimer = DispatchSource.makeTimerSource(flags: [], queue: DispatchQueue.main)

    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
//        let view = UIImageView(frame: CGRect(x: 100, y: 100, width: 100, height: 100))
////        view.contentMode = .scaleAspectFill
//        view.contentMode = .top
//        view.image = UIImage.init(named: "WechatIMG46")
//        view.layer.borderWidth = 2
//        self.view.addSubview(view)
//        view.bounds.origin.y += 200
//        view.bounds.size.width += 100
//
//        let s = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 10))
//        view.addSubview(s)
//        s.backgroundColor = UIColor.red
        
        
        let view = UIScrollView(frame: CGRect(x: 100, y: 100, width: 100, height: 100))
        //        view.contentMode = .scaleAspectFill
//        view.contentMode = .top
//        view.image = UIImage.init(named: "WechatIMG46")
        view.layer.borderWidth = 1
        self.view.addSubview(view)
        view.contentSize = CGSize(width: 300, height: 300)
        view.contentInset = UIEdgeInsets(top: 10, left: 10, bottom: 0, right: 0)

        let s = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 10))
        view.addSubview(s)
        s.backgroundColor = UIColor.red
        
        DispatchQueue.main.async {
            print(view)
            print(view.frame)
            print(view.bounds)

        }
        timer.setEventHandler {
            print(view)
            print(view.frame)
            print(view.bounds)
        }
        timer.schedule(deadline: DispatchTime.now(), repeating: DispatchTimeInterval.seconds(5), leeway: DispatchTimeInterval.seconds(5))
        timer.resume()
        
        
        
    }


}

