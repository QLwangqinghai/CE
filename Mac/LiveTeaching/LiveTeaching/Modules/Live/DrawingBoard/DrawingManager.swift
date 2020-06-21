//
//  Drawing.swift
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


//https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB



//http://blog.sina.com.cn/s/blog_894d45e20102wwrt.html



public class DrawingManager {
    
    private var links: [TimeInterval]
    var displaylink: CADisplayLink?
    
    func createDisplayLink() {
        // 创建CADisplayLink
        displaylink = CADisplayLink(target: self,
                                    selector: #selector(DrawingManager.onDisplaylink))
        displaylink!.add(to: RunLoop.main, forMode: RunLoop.Mode.common)
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
//        print("current:\(CACurrentMediaTime()) linkInfo.current:\(displaylink.timestamp) linkInfo.target\(displaylink.targetTimestamp)")
    }
    
    // 停止CADisplayLink
    func stopDisplaylink() {
        displaylink?.invalidate()
        displaylink = nil
    }
    init() {
        self.links = []
        self.createDisplayLink()
        self.links.reserveCapacity(61)
    }
    
    
    public static let shared: DrawingManager = DrawingManager()
    
}

/*
video: 16 * 9
video: 4 * 3
video: 3 * 2

 3/3/5 45 720
 3/5   60 960
 3/5   75 1200
 3/5   80 1280

 
width: 720、810、960、1080、1280、1440、1620、1920
       6    7   8    9     10    12   13    1
960 * 540
720 * 540
810 * 540

1280 * 720
960 * 720
1080 * 720

1920 * 1080
1440 * 1080
1620 * 1080

//72 54 108  180 * 4 180 * 3 180 * 6

*/
public enum DrawingSize {
    case preset960x540

    //good
    case preset960x720
    case preset1280x720
    case preset1280x1080

    //good
    case preset1440x1080
    case preset1920x1080

    public var rawValue: Size {
        switch self {
        case .preset960x540: return Size(width: 960, height: 540)
        case .preset960x720: return Size(width: 960, height: 720)
        case .preset1280x720: return Size(width: 1280, height: 720)
        case .preset1280x1080: return Size(width: 1280, height: 1080)
        case .preset1440x1080: return Size(width: 1440, height: 1080)
        case .preset1920x1080: return Size(width: 1920, height: 1080)
        }
    }
    public var cgSize: CGSize {
        let s = self.rawValue
        return CGSize(width: CGFloat(s.width) / UIScreen.main.scale, height: CGFloat(s.height) / UIScreen.main.scale)
    }
}



//public struct Manager {
////    public static let context: DrawingContainer = DrawingContainer()
////    public static let specificKey: DispatchSpecificKey<DrawingContainer> = DispatchSpecificKey<DrawingContainer>()
////    public static let queue: DispatchQueue = {
////        let q = DispatchQueue(label: "d", qos: DispatchQoS.userInitiated, attributes: [], autoreleaseFrequency: .inherit, target: nil)
////        q.setSpecific(key: Manager.specificKey, value: Manager.context)
////        return q
////    }()
//
//    
//    
//
//
//}
//
//public class ByteBuffer: NSObject {
//    public let ptr: UnsafeMutableRawPointer
//    public let size: Int
//    public init(ptr: UnsafeMutableRawPointer, size: Int) {
//        assert(size > 0);
//        self.ptr = ptr
//        self.size = size
//    }
//    public init(size: Int) {
//        assert(size > 0);
//        self.ptr = UnsafeMutableRawPointer.allocate(byteCount: size, alignment: CCGetCachelineSize())
//        self.size = size
//    }
//    deinit {
//        self.ptr.deallocate()
//    }
//}
//public class FileContent {
//    public let path: String
//
//    //标识文件是否为空
//    public var meta: ByteBuffer? {
//        didSet(old) {
//            if old != self.meta {
//                FileManager.shared.setNeedsSync(meta: self.meta, to: self.path)
//            }
//        }
//    }
//    public init(path: String, meta: ByteBuffer?) {
//        self.meta = meta
//        self.path = path
//    }
//    public func setNeedsSyncToFile() {
//        FileManager.shared.setNeedsSync(meta: self.meta, to: self.path)
//    }
//}
//
//public class FileManager {
//    /*
//     userId/liveId/dirId/pageId/bg/y
//     userId/liveId/dirId/pageId/drawing/y
//     userId/liveId/dirId/pageId/function/xx/xx
//     */
//    
//    private let lock: NSLock = NSLock()
//    private var dictionary: [String: FileData] = [:]
//    
//    private let markLock: NSLock = NSLock()
//    private var markedRemoves: [FileData] = []
//
//    public func setNeedsRemove(_ item: FileData) {
//        self.markLock.lock()
//        self.markedRemoves.append(item)
//        self.markLock.unlock()
//    }
//    
//    public static let shared: FileManager = FileManager()
//    
//    public func clear() {
//        self.markLock.lock()
//        let items = self.markedRemoves
//        self.markedRemoves.removeAll()
//        self.markLock.unlock()
//        self.lock.lock()
//        for item in items {
//            if item.isDiscarded() {
//                self.dictionary.removeValue(forKey: item.path)
//            }
//        }
//        self.lock.unlock()
//    }
//    
//    public func beginAccessFileData(path: String) -> FileData {
//        self.lock.lock()
//        let result: FileData
//        if let item = self.dictionary[path] {
//            result = item
//        } else {
//            result = FileData(path: path)
//            self.dictionary[path] = result
//        }
//        self.lock.unlock()
//        result.beginAccess()
//        return result
//    }
//    public func endAccess(fileData: FileData) {
//        fileData.endAccess()
//    }
//    
//    public func loadContent(of filePath: String) -> FileContent {
//        return FileContent(path:filePath, meta: nil)
//    }
//
//    public func setNeedsSync(meta: ByteBuffer?, to path: String) {
//        
//    }
//
//}
//

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
    private var level: (Int64, Int) = (0, 0) {
       didSet(old) {
           let new = self.level
           if old != new {
            print("fps time:\(new.0) level:\(new.1)")
           }
       }
   }
    private var fps: (Int64, Int) = (0, 0) {
        didSet(oldValue) {
            let old = oldValue.1
            let new = self.fps.1
            if old != new {
                if new >= 58 {
                    self.level = (self.fps.0, 10)
                } else if new >= 55 {
                    self.level = (self.fps.0, 9)
                } else if new >= 52 {
                    self.level = (self.fps.0, 8)
                } else if new >= 49 {
                    self.level = (self.fps.0, 7)
                } else if new >= 45 {
                    self.level = (self.fps.0, 6)
                } else if new >= 40 {
                    self.level = (self.fps.0, 5)
                } else if new >= 30 {
                    self.level = (self.fps.0, 4)
                } else if new >= 20 {
                    self.level = (self.fps.0, 3)
                } else if new >= 10 {
                    self.level = (self.fps.0, 2)
                } else if new >= 1 {
                    self.level = (self.fps.0, 1)
                } else {
                    self.level = (self.fps.0, 0)
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
    
    @objc func onDisplaylink(displaylink: CADisplayLink) {
        self.check(timestamp: displaylink.timestamp)
    }
    func check(timestamp: TimeInterval) {
        let last = Int64(timestamp)
        if last != self.last {
            if self.last + 1 == last {
                self.fps = (self.last, self.links.count)
                self.links.removeAll()
                self.last = last
            } else {
                self.fps = (last - 1, 0)
                self.links.removeAll()
                self.last = last
            }
        }
        self.links.append(timestamp)
    }
    func stopDisplaylink() {
        displaylink?.invalidate()
        displaylink = nil
    }
    init() {
        self.links = [0]
        self.createDisplayLink()
        self.links.reserveCapacity(60)
    }
    public static let shared: DisplayManager = DisplayManager()
}

