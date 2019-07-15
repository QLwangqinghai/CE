//
//  ViewController.swift
//  Demo
//
//  Created by vector on 2019/7/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import AVFoundation
import ImageIO
import Photos

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        W.go()
        
        let url = URL(fileURLWithPath: "/tmp/output.heic")
        
        guard let image = UIImage(named: "p.png") else {
            fatalError("unable to create CGImageDestination")
        }
        guard let imageDestination =  CGImageDestinationCreateWithURL(url as CFURL, AVFileType.heic.rawValue as CFString, 1, nil) else{
                
            fatalError("unable to create CGImageDestination")
        }
        
        CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
        
        CGImageDestinationFinalize(imageDestination)

        
    }


}

