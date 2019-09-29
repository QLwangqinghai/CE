//
//  ViewController.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UIScrollViewDelegate {

    let zoomView: ZoomView = ZoomView()
    
    let vv: UIView = UIView()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let scrollView = UIScrollView(frame: CGRect(origin: CGPoint(x: 10, y: 150), size: CGSize(width: 500, height: 140)))
        scrollView.delegate = self
        scrollView.contentInset = UIEdgeInsets(top: 0, left: 10, bottom: 0, right: 0)
        scrollView.backgroundColor = UIColor.green
        self.view.addSubview(scrollView)
        scrollView.contentSize = CGSize(width: 2000, height: 100)
        
        
        let imageView1 = UIView(frame: CGRect(origin: CGPoint(x: 400, y: 0), size: CGSize(width: 500, height: 100)))
        scrollView.addSubview(imageView1)
        imageView1.backgroundColor = UIColor.red
        
        let v = UIScrollView(frame: CGRect(origin: CGPoint(x: 300, y: 0), size: CGSize(width: 80, height: 100)))
        v.bounces = false
        v.contentSize = CGSize(width: 700, height: 100)
        scrollView.addSubview(v)
        v.backgroundColor = UIColor.black
            
//        scrollView.panGestureRecognizer.delegate = self
//        v.panGestureRecognizer.delegate = self
        
        let v1 = UIView(frame: CGRect(origin: CGPoint(x: 100, y: 200), size: CGSize(width: 150, height: 250)))
        self.view.addSubview(v1)
        v1.backgroundColor = .blue
        v1.transform = CGAffineTransform(rotationAngle: CGFloat(Double.pi/2))
        
        let button: UIButton = UIButton(type: .custom)
        v1.addSubview(button)
        button.backgroundColor = .red
        button.frame = CGRect.init(x: 0, y: 0, width: 40, height: 40)
        
        button.addTarget(self, action: #selector(clicked), for: .touchUpInside)
        
        
        self.view.addSubview(self.zoomView)
        
        self.zoomView.frame = self.view.bounds // CGRect(origin: CGPoint(x: 50, y: 500), size: CGSize(width: 150, height: 250))
        self.zoomView.layer.borderColor = UIColor.red.cgColor
        self.zoomView.layer.borderWidth = 2.0
        self.zoomView.scrollView.frame = CGRect(origin: CGPoint(x: 20, y: 20), size: CGSize(width: 100, height: 160))
        
        let rotateButton: UIButton = UIButton(type: .custom)
        self.view.addSubview(rotateButton)
        rotateButton.backgroundColor = .gray
        rotateButton.frame = CGRect.init(x: 0, y: 600, width: 40, height: 40)
        rotateButton.addTarget(self, action: #selector(rotateButtonClicked), for: .touchUpInside)
        
//        let imageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 1000, height: 600))
        let imageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 100, height: 60))
        imageView.image = UIImage(named: "10.jpg")
        self.zoomView.contentView.addSubview(imageView)
        self.zoomView.scrollView.backgroundColor = .black
        self.zoomView.originalContentSize = imageView.frame.size
//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
//            self.zoomView.transform = CGAffineTransform.init(scaleX: 0.8, y: 0.8)
//        }
        
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

    @objc func rotateButtonClicked() {
        self.zoomView.rotate(direction: OrientationView.Direction.anticlockwise, duration: 0.2) { (result) in
            print("rotate result: \(result)")
        }
//        print("rotateButtonClicked")
    }
    
    



}

