//
//  ViewController.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UIScrollViewDelegate {

    var zoomController: ZoomScrollController?
    var zoomView: OrientationView?
    
    let vv: UIView = UIView()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        let zoomController = ZoomScrollController()
        self.zoomController = zoomController
        let zoomView = OrientationView(frame: self.view.bounds)
        zoomView.updateContent(zoomController)
        self.zoomView = zoomView

        
        
        
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
        
        
        self.view.addSubview(zoomView)
        zoomView.layer.borderColor = UIColor.red.cgColor
        zoomView.layer.borderWidth = 2.0
        
        let rotateButton: UIButton = UIButton(type: .custom)
        self.view.addSubview(rotateButton)
        rotateButton.backgroundColor = .gray
        rotateButton.frame = CGRect.init(x: 0, y: 600, width: 40, height: 40)
        rotateButton.addTarget(self, action: #selector(rotateButtonClicked), for: .touchUpInside)
        
//        let imageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 1000, height: 600))
        let imageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 100, height: 60))
        zoomController.updateContentSize(CGSize(width: 100, height: 60))
        imageView.image = UIImage(named: "10.jpg")
        if let orientationContentView = zoomView.content?.orientationContentView {
            orientationContentView.addSubview(imageView)
            orientationContentView.backgroundColor = .black            
        }
//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
//            self.zoomView.transform = CGAffineTransform.init(scaleX: 0.8, y: 0.8)
//        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
            let view = self.zoomView!
            
            UIView.animate(withDuration: 3, animations: {
                view.frame = CGRect.init(x: 100, y: 100, width: 100, height: 100)
            }) { (result) in
                
            }
            

        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 7) {
            let view = self.zoomView!
            
            UIView.animate(withDuration: 3, animations: {
                view .updateFrame(frame: self.view.bounds) { (layout) -> [AnyHashable : Any] in
                    return [:]
                }
            }) { (result) in
                
            }

        }
        
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
        self.zoomView!.updateContentLayout(duration: 0.5) { (layout) -> [AnyHashable : Any] in
            layout.rotate(direction: .anticlockwise)
            return [:]
        }
//        self.zoomView.rotate(direction: OrientationView.Direction.anticlockwise, option: [:], duration: 0.2) { (result) in
//            print("rotate result: \(result)")
//        }
//        print("rotateButtonClicked")
    }
    
    



}

