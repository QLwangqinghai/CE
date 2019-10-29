//
//  ViewController.swift
//  Linear
//
//  Created by vector on 2019/10/29.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let stackView = UIStackView(frame: CGRect(x: 0, y: 100, width: 300, height: 100))
        self.view.addSubview(stackView)
        
        let v = UIView()
        v.backgroundColor = .red
        stackView.addArrangedSubview(v)
        
        let layer: CAShapeLayer = CAShapeLayer()
        layer.frame = stackView.bounds
        layer.path = UIBezierPath(roundedRect: stackView.bounds, cornerRadius: 20).cgPath
        stackView.layer.mask = layer

        
    }


}

