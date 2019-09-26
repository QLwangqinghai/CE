//
//  ViewController.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UIScrollViewDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let scrollView = UIScrollView(frame: CGRect(origin: CGPoint(x: 10, y: 150), size: CGSize(width: 500, height: 140)))
        scrollView.delegate = self
        scrollView.contentInset = UIEdgeInsets(top: 0, left: 10, bottom: 0, right: 0)
        scrollView.backgroundColor = UIColor.green
        self.view.addSubview(scrollView)
        scrollView.contentSize = CGSize(width: 2000, height: 100)
        
        
        let imageView = UIView(frame: CGRect(origin: CGPoint(x: 400, y: 0), size: CGSize(width: 500, height: 100)))
        scrollView.addSubview(imageView)
        imageView.backgroundColor = UIColor.red
        
        let v = UIScrollView(frame: CGRect(origin: CGPoint(x: 300, y: 0), size: CGSize(width: 80, height: 100)))
        v.bounces = false
        v.contentSize = CGSize(width: 700, height: 100)
        scrollView.addSubview(v)
        v.backgroundColor = UIColor.black
            
//        scrollView.panGestureRecognizer.delegate = self
//        v.panGestureRecognizer.delegate = self
        
        let v1 = UIView(frame: CGRect(origin: CGPoint(x: 100, y: 300), size: CGSize(width: 150, height: 250)))
        self.view.addSubview(v1)
        v1.backgroundColor = .blue
        v1.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi/2))
        
        let button: UIButton = UIButton(type: .custom)
        v1.addSubview(button)
        button.backgroundColor = .red
        button.frame = CGRect.init(x: 0, y: 0, width: 40, height: 40)
        
        button.addTarget(self, action: #selector(clicked), for: .touchUpInside)
    }
    

    func scrollViewDidScroll(_ scrollView: UIScrollView) {
//        print(scrollView)
//        print(scrollView.bounds)
//        print(scrollView.contentSize)
//        print(scrollView.contentOffset)
    }

    @objc func clicked() {
        print("clicked")
    }

    



}

