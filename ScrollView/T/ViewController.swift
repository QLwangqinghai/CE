//
//  ViewController.swift
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


/*
 　6、A8采用了最先进的20nm工艺制造（台积电代工），面积更小巧，能耗比更为出色，也同时继承了专门的M8运动协处理器；相比于A7，A8在CPU性能提升25%，图形性能提高了50%；A8X集成多达30亿个晶体管，介于NVIDIA GK104 35亿个、GK106 25亿个之间，在CPU上采用三核心设计比A7提升了40%，2GB的内存，GPU型号为PowerVR GX6850；
  6 6p
 
 
 　　7、A9处理器拥有双版本和代工厂，6s为APL0898，封装的是三星2GB LPDDR4 RAM由三星代工，14nm；6s Plus 的 A9 处理器型号为APL1022 ，封装的是海力士 2GB LPDDR4 RAM，由台积电代工，16nm；A9是A8性能的130%，单核的跑分为2526分，多核为4404分；A9X回到双核心设计，GPU采用6个GPU单元，总共12个GPU核心，用有384个流处理器；
 
 2015  6s 6sp se

 　　8、A10内核上的编号为TMGK98，延续了A9 TMGK96，而核心面积大约125平方毫米，封装使用了台积电最新的InFO技术；A10 Fusion性能是第一代iPhone芯片的120倍，比iPhone 6s中的A9提升40%。同时这是A系列的首个四核处理器，采用的是两个大核和两个小核的设计；　高性能核心的运行速度最高可达iPhone 6 的2倍，而高能效核心在运行时的功率则可低至高性能核心的五分之一；

 2016 7 7p
 
 　　9、A11采用台积电最先进的10nm工艺制程，采用六核心的设计，大河性能相比A10提升25%，4颗小核相较A10提升70%，多性能处理提升75%；搭载的GPU是苹果自研的三核心 GPU ，性能较 A10 性能提升 30%，而功耗则降低了 50%；首次搭载神经网络引擎，用于AR和图像识别；

 2017 8 8p X

 　　10、A12的处理器采用最新的7nm的制程，使用的是苹果自研Fusion架构，均为2(性能核心，性能提升15%)+4(能效核心，功耗表现提升50%)；GPU采用新一代自研GPU，核心从三核升级到四核，官方性能提升了50%；神经网络从双核升级为八核能够实现50000亿次计算次数；根据苹果的数据，2个大核提升15%，4个小核功耗表现提升50%；采用了新一代自研GPU，核心数升级为4核，性能提升50%；
 
 2018 XSMax

 */

public class Accelerator {
    public class TaskQueue {
        public private(set) var tasks: [Worker]

        public let semaphore: DispatchSemaphore
        
        public init() {
            let semaphore: DispatchSemaphore = DispatchSemaphore(value: 0)

            var workers: [Worker] = []
            let mainWorker = Worker(semaphore: semaphore)
            workers.append(mainWorker)
            let thread = Thread(target: mainWorker, selector: #selector(Worker.run), object: nil)
            thread.threadPriority = 1.0
            thread.start()
            let count = ProcessInfo.processInfo.activeProcessorCount - 2
            if count > 0 {
                for _ in 0 ..< count {
                    
                }
                let worker = Worker(semaphore: semaphore)
                workers.append(worker)
                let thread = Thread(target: worker, selector: #selector(Worker.run), object: nil)
                thread.threadPriority = 1.0
                thread.start()
            }
            self.semaphore = semaphore
            self.workers = workers
        }
        
        
    }

    
    
    public class Pool {
        public let workers: [Worker]

        public let semaphore: DispatchSemaphore
        
        public init() {
            let semaphore: DispatchSemaphore = DispatchSemaphore(value: 0)

            var workers: [Worker] = []
            let mainWorker = Worker(semaphore: semaphore)
            workers.append(mainWorker)
            let thread = Thread(target: mainWorker, selector: #selector(Worker.run), object: nil)
            thread.threadPriority = 1.0
            thread.start()
            let count = ProcessInfo.processInfo.activeProcessorCount - 2
            if count > 0 {
                for _ in 0 ..< count {
                    
                }
                let worker = Worker(semaphore: semaphore)
                workers.append(worker)
                let thread = Thread(target: worker, selector: #selector(Worker.run), object: nil)
                thread.threadPriority = 1.0
                thread.start()
            }
            self.semaphore = semaphore
            self.workers = workers
        }
        
        
    }
    public class Worker: NSObject {
        public let semaphore: DispatchSemaphore
        public init(semaphore: DispatchSemaphore) {
            self.semaphore = semaphore
            super.init()
        }
        @objc func run() {
            
        }
    }
    
    

    let queue: DispatchQueue = DispatchQueue(label: "acceleration", qos: DispatchQoS.userInitiated, attributes: DispatchQueue.Attributes.concurrent, autoreleaseFrequency: .inherit, target: nil)
    
    func aa() {
        let g = DispatchGroup()
        g.enter()
        
        let semaphore: DispatchSemaphore = DispatchSemaphore(value: 0)
        semaphore.signal()
    }
    
    let pool: Pool
    
    public init() {
        self.pool = Pool()
    }
    
}

public class DisplayManager {

    private var links: [TimeInterval]
    private var last: Int64 = 0
    
    /*
     10 58 +
     9 55 - 57
     8 52 - 54
     7 49 - 51
     6 45 - 48
     5 40 - 44
     4 30 - 39
     3 20 - 29
     2 10 - 19
     1 1 - 9
     0 0
     */
    private var level: Int = 0 {
       didSet(old) {
           let new = self.level
           if old != new {
               print("fps level:\(level)")
           }
       }
   }
    private var fps: Int = 0 {
        didSet(old) {
            let new = self.fps
            if old != new {
                if new >= 58 {
                    self.level = 10
                } else if new >= 55 {
                    self.level = 9
                } else if new >= 52 {
                    self.level = 8
                } else if new >= 49 {
                    self.level = 7
                } else if new >= 45 {
                    self.level = 6
                } else if new >= 40 {
                    self.level = 5
                } else if new >= 30 {
                    self.level = 4
                } else if new >= 20 {
                    self.level = 3
                } else if new >= 10 {
                    self.level = 2
                } else if new >= 1 {
                    self.level = 1
                } else {
                    self.level = 0
                }
            }
        }
    }

    var displaylink: CADisplayLink?
    
    func createDisplayLink() {
        // 创建CADisplayLink
        let displaylink = CADisplayLink(target: self,
                                    selector: #selector(DisplayManager.onDisplaylink))
        if #available(iOS 10, *) {
            displaylink.preferredFramesPerSecond = 60
        } else {
            displaylink.frameInterval = 1
        }
        displaylink.add(to: RunLoop.main, forMode: RunLoop.Mode.common)
        self.displaylink = displaylink
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

    
    
    @objc func onDisplaylink(displaylink: CADisplayLink) {
        // 打印时间戳
//        print("current:\(CACurrentMediaTime()) linkInfo.current:\(displaylink.timestamp) linkInfo.target\(displaylink.targetTimestamp) preferredFramesPerSecond:\(displaylink.preferredFramesPerSecond)")
        
        self.check(timestamp: displaylink.timestamp)
    }
    func check(timestamp: TimeInterval) {
        let last = Int64(timestamp)
        if last != self.last {
            self.fps = self.links.count
            self.links.removeAll()
            self.last = last
        }
        self.links.append(timestamp)
    }
    
    // 停止CADisplayLink
    func stopDisplaylink() {
        displaylink?.invalidate()
        displaylink = nil
    }
    
    
    init() {
        self.links = [0]
        self.createDisplayLink()
        self.links.reserveCapacity(60)
        
        
        ProcessInfo.processInfo.activeProcessorCount
        
    }
    
    public static let shared: DisplayManager = DisplayManager()
}


class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
//        UIColor.init(patternImage: UIImage)
        
        let _ = DisplayManager.shared
        
        let mem = TMemory(sizeInMb: 256)
        mem.dispatchSignal()
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 5) {
            mem.testMemcpy()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 8) {
            mem.testMemcpy()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 11) {
            mem.testMemcpy()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 14) {
            mem.testMemcpy()
        }
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 17) {
            mem.testMemcpy()
        }
    }


}

