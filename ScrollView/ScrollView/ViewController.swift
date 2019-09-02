//
//  ViewController.swift
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Metal

public struct TimingBlock {
    
    
    public static func `do`(_ block:() -> Void) {
        let b = CACurrentMediaTime()
        block()
        let e = CACurrentMediaTime()
        let time = UInt64((e - b) * 1000)
        
        let sec = time / 10000
        let ms = time % 10000 / 10
        let ms2 = time % 10
        let str = String(format: "%ld.%03ld.%01ld", sec, ms, ms2)
        print("used: \(str)")
    }
}

/*
 current:177796.438314261 linkInfo.current:177796.43738376902 linkInfo.target177796.45405043603
 current:177796.454138153 linkInfo.current:177796.454050436 linkInfo.target177796.47071710302
 current:177796.471807141 linkInfo.current:177796.47071710302 linkInfo.target177796.48738377003
 current:177796.48855514202 linkInfo.current:177796.48738377 linkInfo.target177796.504050437
 current:177796.504278598 linkInfo.current:177796.504050437 linkInfo.target177796.52071710402
 current:177796.521790495 linkInfo.current:177796.52071710402 linkInfo.target177796.53738377104
 current:177796.538562163 linkInfo.current:177796.537383771 linkInfo.target177796.55405043802
 current:177796.555210947 linkInfo.current:177796.55405043802 linkInfo.target177796.57071710503
 current:177796.57179016102 linkInfo.current:177796.570717105 linkInfo.target177796.587383772
 current:177796.588546051 linkInfo.current:177796.587383772 linkInfo.target177796.60405043903
 current:177796.60522652502 linkInfo.current:177796.604050439 linkInfo.target177796.620717106
 current:177796.620782698 linkInfo.current:177796.620717106 linkInfo.target177796.63738377302
 current:177796.637515019 linkInfo.current:177796.63738377302 linkInfo.target177796.65405044003
 current:177796.65511287202 linkInfo.current:177796.65405044 linkInfo.target177796.67071710702

 
 */

class ViewController: UIViewController {
    let timer: DispatchSourceTimer = DispatchSource.makeTimerSource(flags: [], queue: DispatchQueue.main)
//    public func setViewports(_ viewports: [MTLViewport])
//
//    @available(OSX 10.13, iOS 12.0, *)
//    public func setScissorRects(_ scissorRects: [MTLScissorRect])
    
    var displaylink: CADisplayLink?
    
    func createDisplayLink() {
        // 创建CADisplayLink
        displaylink = CADisplayLink(target: self,
                                    selector: #selector(step))
        
        displaylink!.add(to: .current,
                         forMode: RunLoop.Mode.default)
    }
    
    
//    open var timestamp: CFTimeInterval { get }
//
//    open var duration: CFTimeInterval { get }
//
//
//    /* The next timestamp that the client should target their render for. */
//
//    @available(iOS 10.0, *)
//    open var targetTimestamp: CFTimeInterval { get }

    @objc func step(displaylink: CADisplayLink) {
        // 打印时间戳
        print("current:\(CACurrentMediaTime()) linkInfo.current:\(displaylink.timestamp) linkInfo.target\(displaylink.targetTimestamp)")
    }
    
    // 停止CADisplayLink
    func stopDisplaylink() {
        displaylink?.invalidate()
        displaylink = nil
    }
    
    var a: UnsafeMutableRawPointer? = nil
    var b: UnsafeMutableRawPointer? = nil
    var c: UnsafeMutableRawPointer? = nil

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.

        TimingBlock.do {
            a = ContextCreate()
        }
        TimingBlock.do {
            b = ContextCreate();
        }
//        c = ContextCreate();

        
        
        self.createDisplayLink()
        let _ = self.view.layer
        
        let imageView = UIImageView(frame: CGRect(x: 110, y: 100, width: -100, height: 300))
        imageView.clipsToBounds = true;
//        view.contentMode = .scaleAspectFill
        imageView.contentMode = .scaleAspectFill
        imageView.image = UIImage(named: "WechatIMG46")
        imageView.layer.borderWidth = 2
        self.view.addSubview(imageView)
        imageView.isUserInteractionEnabled = true
        
        let v1 = UIView(frame: CGRect(x: 0, y: 0, width: -30, height: 30))
        v1.backgroundColor = .red
        imageView.addSubview(v1)
        
        let v2 = UIView(frame: CGRect(x: 0, y: 0, width: 30, height: 30))
        v2.backgroundColor = .green
        imageView.addSubview(v2)

        let tap: UITapGestureRecognizer = UITapGestureRecognizer.init(target: self, action:#selector(tapped(tap:)))
        imageView.addGestureRecognizer(tap)
//
//        let s = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 10))
//        view.addSubview(s)
//        s.backgroundColor = UIColor.red
        
        
        let view = UIScrollView(frame: CGRect(x: 230, y: 300, width: 100, height: 100))
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
        return
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

    @objc func tapped(tap: UITapGestureRecognizer) {
        print(tap)
    }

}

