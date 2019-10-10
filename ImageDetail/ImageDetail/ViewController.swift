//
//  ViewController.swift
//  ImageDetail
//
//  Created by vector on 2019/9/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


//class ViewController2: UIViewController {
//    var zoomView: OrientationView?
//    var zoomController: ZoomController?
//
//    let vv: UIView = UIView()
//
//
//    override func viewDidLoad() {
//        super.viewDidLoad()
//        self.view.backgroundColor = .white
//        // Do any additional setup after loading the view.
//        let zoomController = ZoomController()
//        self.zoomController = zoomController
//        let zoomView = OrientationView(frame: self.view.bounds)
//        zoomView.updateContent(zoomController)
//        self.zoomView = zoomView
//
//        self.view.addSubview(zoomView)
//        zoomView.layer.borderColor = UIColor.red.cgColor
//        zoomView.layer.borderWidth = 2.0
//
//        let rotateButton: UIButton = UIButton(type: .custom)
//        self.view.addSubview(rotateButton)
//        rotateButton.backgroundColor = .gray
//        rotateButton.frame = CGRect.init(x: 0, y: 600, width: 40, height: 40)
//        rotateButton.addTarget(self, action: #selector(rotateButtonClicked), for: .touchUpInside)
//
//        let imageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 1000, height: 600))
////        let imageView: UIImageView = UIImageView()
//
//        zoomController.updateZoomLayout { (layout) in
//            layout.contentSize = CGSize(width: 100, height: 60)
//            layout.minimumZoomScale = 1
//            layout.maximumZoomScale = 20
//        }
//        zoomController.contentView.image = UIImage(named: "10.jpg")
//        zoomController.orientationContentView;
////        imageView.image = UIImage(named: "10.jpg")
////        imageView.translatesAutoresizingMaskIntoConstraints = false
////        zoomController.contentView.addSubview(imageView)
////
////        let heightConstraint = NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.height, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomController.contentView, attribute: NSLayoutConstraint.Attribute.height, multiplier: 1.0, constant: 0)
////        heightConstraint.priority = UILayoutPriority.required
////        zoomController.contentView.addConstraint(heightConstraint)
////
////        let widthConstraint = NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.width, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomController.contentView, attribute: NSLayoutConstraint.Attribute.width, multiplier: 1.0, constant: 0)
////        widthConstraint.priority = UILayoutPriority.required
////        zoomController.contentView.addConstraint(widthConstraint)
////
////        zoomController.contentView.addConstraint(NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.left, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomController.contentView, attribute: NSLayoutConstraint.Attribute.left, multiplier: 1.0, constant: 0))
////        zoomController.contentView.addConstraint(NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.top, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomController.contentView, attribute: NSLayoutConstraint.Attribute.top, multiplier: 1.0, constant: 0))
//
//
//
//        zoomController.contentView.backgroundColor = .black
//
//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 4) {
//            let view = self.zoomView!
//
//            UIView.animate(withDuration: 3, animations: {
//                view .updateFrame(frame: self.view.bounds) { (layout) -> [AnyHashable : Any] in
//                    return [:]
//                }
//            }) { (result) in
//
//            }
//
//        }
//
//    }
//
//    @objc func rotateButtonClicked() {
//        self.zoomView!.updateContentLayout(duration: 0.5) { (layout) -> [AnyHashable : Any] in
//            layout.rotate(direction: .anticlockwise)
//            return [:]
//        }
//
//
//
//
//    }
//
//
//
//}
//
//

class ViewController: UIViewController, UIScrollViewDelegate {

    var zoomView: OrientationView?
    var zoomScrollController: ZoomScrollController?

    let vv: UIView = UIView()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        let zoomScrollController = ZoomScrollController()
        self.zoomScrollController = zoomScrollController
        let zoomView = OrientationView(frame: self.view.bounds)
        zoomView.updateContent(zoomScrollController)
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
        let imageView: UIImageView = UIImageView()
        
        zoomScrollController.updateZoomLayout { (layout) in
            layout.originalSize = CGSize(width: 100, height: 60)
//            layout.minimumZoomScale = 1
//            layout.maximumZoomScale = 20
        }

        imageView.image = UIImage(named: "10.jpg")
        imageView.translatesAutoresizingMaskIntoConstraints = false
        zoomScrollController.contentView.addSubview(imageView)
        
        let heightConstraint = NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.height, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomScrollController.contentView, attribute: NSLayoutConstraint.Attribute.height, multiplier: 1.0, constant: 0)
        heightConstraint.priority = UILayoutPriority.required
        zoomScrollController.contentView.addConstraint(heightConstraint)
        
        let widthConstraint = NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.width, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomScrollController.contentView, attribute: NSLayoutConstraint.Attribute.width, multiplier: 1.0, constant: 0)
        widthConstraint.priority = UILayoutPriority.required
        zoomScrollController.contentView.addConstraint(widthConstraint)

        zoomScrollController.contentView.addConstraint(NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.left, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomScrollController.contentView, attribute: NSLayoutConstraint.Attribute.left, multiplier: 1.0, constant: 0))
        zoomScrollController.contentView.addConstraint(NSLayoutConstraint(item: imageView, attribute: NSLayoutConstraint.Attribute.top, relatedBy: NSLayoutConstraint.Relation.equal, toItem: zoomScrollController.contentView, attribute: NSLayoutConstraint.Attribute.top, multiplier: 1.0, constant: 0))

        
        
        zoomScrollController.contentView.backgroundColor = .black

//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
//            self.zoomView.transform = CGAffineTransform.init(scaleX: 0.8, y: 0.8)
//        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 1) {
            let view = self.zoomView!
            
            UIView.animate(withDuration: 0.25, animations: {
                view.frame = CGRect.init(x: 100, y: 100, width: 100, height: 100)
            }) { (result) in
                
            }
            

        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            let view = self.zoomView!
            
            UIView.animate(withDuration: 0.25, animations: {
                view .updateFrame(frame: self.view.bounds) { (layout) -> [AnyHashable : Any] in
                    return [:]
                }
            }) { (result) in
                
            }

        }
        
    }
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
//        self.present(ViewController2(), animated: true) {
//
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

