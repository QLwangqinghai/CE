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
        
//        print("\(Date().timeIntervalSince1970)")
//        W.go()
        
        
        print("\(Date().timeIntervalSince1970)")

        let url = URL(fileURLWithPath: "/tmp/output.heic")
        
//        guard let image = UIImage(named: "png1.png") else {
//            fatalError("unable to create CGImageDestination")
//        }
        
        guard let image = UIImage(named: "p.png") else {
            fatalError("unable to create CGImageDestination")
        }

        
//        guard let imageDestination =  CGImageDestinationCreateWithURL(url as CFURL, AVFileType.heic.rawValue as CFString, 1, nil) else {
//
//            fatalError("unable to create CGImageDestination")
//        }
//        CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
//        CGImageDestinationFinalize(imageDestination)

        
//        let _ = { () -> Int in
//            let mutableData = NSMutableData()
//            guard let imageDestination =  CGImageDestinationCreateWithData(mutableData as CFMutableData, AVFileType.heic.rawValue as CFString, 1, nil) else {
//                fatalError("unable to create CGImageDestination")
//            }
//            CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
//            CGImageDestinationFinalize(imageDestination)
//
//            print("len : \(mutableData.length)")
//
//            return 1
//        }()
        print("\(Date().timeIntervalSince1970)")

//        let _ = { () -> Int in
//            let mutableData = NSMutableData()
//            guard let imageDestination =  CGImageDestinationCreateWithData(mutableData as CFMutableData, AVFileType.avci.rawValue as CFString, 1, nil) else {
//                fatalError("unable to create CGImageDestination ")
//            }
//            CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
//            CGImageDestinationFinalize(imageDestination)
//            
//            print("len : \(mutableData.length)")
//            return 1
//        }()
//
//        
//        let _ = { () -> Int in
//            let mutableData = NSMutableData()
//            guard let imageDestination =  CGImageDestinationCreateWithData(mutableData as CFMutableData, AVFileType.heif.rawValue as CFString, 0, nil) else {
//                fatalError("unable to create CGImageDestination ")
//            }
//            CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
//            CGImageDestinationFinalize(imageDestination)
//            
//            print("len : \(mutableData.length)")
//            return 1
//        }()
        
        print("\(Date().timeIntervalSince1970)")
        
        
        let mySourceTypes = CGImageSourceCopyTypeIdentifiers();
        CFShow(mySourceTypes);
        let myDestinationTypes = CGImageDestinationCopyTypeIdentifiers();
        CFShow(myDestinationTypes);
    }


}

