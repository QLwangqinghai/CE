//
//  AppDelegate.swift
//  mDemo
//
//  Created by vector on 2019/7/16.
//  Copyright © 2019 angfung. All rights reserved.
//

import Cocoa
import AVFoundation
import ImageIO
import Photos
import SDWebImageHEIFCoder

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!


    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        let libraryDirectory = NSSearchPathForDirectoriesInDomains(.libraryDirectory, .userDomainMask, true).first!

        let path = libraryDirectory + "/a.png"
        print("path: \(path)")
        
        
        let mySourceTypes = CGImageSourceCopyTypeIdentifiers();
        CFShow(mySourceTypes);
        let myDestinationTypes = CGImageDestinationCopyTypeIdentifiers();
        CFShow(myDestinationTypes);
        
        
        let ciImage = CIImage(contentsOf: URL.init(fileURLWithPath: path))
       

        
        let _ = { () -> Int in
            let mutableData = NSMutableData()
            guard let imageDestination =  CGImageDestinationCreateWithData(mutableData as CFMutableData, AVFileType.heic.rawValue as CFString, 11, nil) else {
                fatalError("unable to create CGImageDestination ")
            }
//            kCGImagePropertyHEICSDelayTime
            for i in 1...11 {
                let image = NSImage(named: "\(i)")!
                let cgImage = image.cgImage(forProposedRect: nil, context: nil, hints: nil)!
                CGImageDestinationAddImage(imageDestination, cgImage, nil)
            }
            
            let result = CGImageDestinationFinalize(imageDestination)
            
            
            mutableData.write(toFile: path, atomically: true)
            
            let ciImage = CIImage(data: mutableData as Data)
            let img = NSImage(data: mutableData as Data)

            
            print("result:\(result) len : \(mutableData.length)")
            return 1
        }()
        
        let spath = Bundle.main.path(forResource: "s", ofType: "heics")!
        let sdata = try! Data.init(contentsOf: URL.init(fileURLWithPath: spath))
        
        let simage = SDImageHEIFCoder.shared.decodedImage(with: sdata, options: nil)!
        print("\(simage.representations)")
        
        
        let image = NSImage(named: "s.heics")!
        print("\(image.representations)")

        
        
//        let data = SDImageHEIFCoder.sharedCoder.en
//        NSData * d = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image format:SDImageFormatHEIF options:NULL];
//            NSLog(@" %ld", d.length);
//
//        NSData * d1 = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image1 format:SDImageFormatHEIF options:NULL];

        
        
//        let image = NSImage(named: "png1.png")!
//        let data = SDImageHEIFCoder.sharedCoder.en
//        NSData * d = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image format:SDImageFormatHEIF options:NULL];
//            NSLog(@" %ld", d.length);
//
//        NSData * d1 = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image1 format:SDImageFormatHEIF options:NULL];
//
//        NSLog(@" %ld", d1.length);
//
//        W.go()
        
        
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

