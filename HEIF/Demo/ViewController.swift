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
import SDWebImage
import SDWebImageHEIFCoder

/*
 2048 × 1536
 32位机器 
 used 137.53801012039185
 used 6.422904014587402

 
 
 
 iphone6p(A8)
 len : 1163708
 used 111.16560101509094
 used 6.005786180496216
 
 iphonese(A9)
 len : 1163708
 used 60.43490815162659
 used 2.653425931930542
 
 ipad 5th gen(A9)
 len : 1163708
 used 56.555789947509766
 used 2.5691940784454346
 
 
 */


class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let mySourceTypes = CGImageSourceCopyTypeIdentifiers();
        CFShow(mySourceTypes);
        let myDestinationTypes = CGImageDestinationCopyTypeIdentifiers();
        CFShow(myDestinationTypes);
        
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
        let begin = Date().timeIntervalSince1970
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

//        let _ = { () -> Int in
//            let mutableData = NSMutableData()
//            guard let imageDestination =  CGImageDestinationCreateWithData(mutableData as CFMutableData, AVFileType.heic.rawValue as CFString, 1, nil) else {
//                fatalError("unable to create CGImageDestination ")
//            }
//
//            for i in 1...11 {
//                let image = UIImage(named: "\(i)")!
//                CGImageDestinationAddImage(imageDestination, image.cgImage!, nil)
//            }
//
//            let result = CGImageDestinationFinalize(imageDestination)
//
//            print("result:\(result) len : \(mutableData.length)")
//            return 1
//        }()
        
        let aimage = UIImage(named: "png1.png")!
        let adata = SDImageHEIFCoder.shared.encodedData(with: aimage, format: SDImageFormat.HEIF, options: nil)
        print("len : \(adata?.count)")
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
        
        let end = Date().timeIntervalSince1970
        print("used \(end - begin)")
        
        SDImageHEIFCoder.shared.decodedImage(with: adata, options: nil)
        let end1 = Date().timeIntervalSince1970
        print("used \(end1 - end)")
    }


}

