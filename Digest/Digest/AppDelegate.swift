//
//  AppDelegate.swift
//  Digest
//
//  Created by vector on 2019/7/27.
//  Copyright © 2019 angfung. All rights reserved.
//

import Cocoa
import CommonCrypto


public class WorkItem {
    let tag: String
    let workItem: DispatchWorkItem
    var time: TimeInterval = 0
    public init(tag: String, block: @escaping @convention(block) () -> Void) {
        self.tag = tag
        self.workItem = DispatchWorkItem(block: block)
    }
    
    public func perform() {
        let b = CFAbsoluteTimeGetCurrent();
        self.workItem.perform()
        let e = CFAbsoluteTimeGetCurrent();
        self.time = e - b
        print("\(tag): used_time:\(self.time)")
    }
    
}

extension Data {
    
    
    public var bytes: Array<UInt8> {
        return Array(self)
    }
    
    public func toHexString() -> String {
        return bytes.toHexString()
    }
}
extension Array where Element == UInt8 {
    
    public func toHexString() -> String {
        return `lazy`.reduce("") {
            var s = String($1, radix: 16)
            if s.count == 1 {
                s = "0" + s
            }
            return $0 + s
        }
    }
}


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
    @IBOutlet weak var window: NSWindow!
    
    //sha 224
    //64mb 122.50645196437836/2, 95.70805394649506/2, 65.84227406978607/2
    //32mb 63.18473494052887/2, 49.14368402957916/2, 16.29863202571869/2
    
    
    //sha 224
    
    var items: [WorkItem] = []
    var results: [String] = []
    
    
    func aa() throws {
        //fec234e24b0a8e908217d21f6e2c4e379eb087ed0c8cc02d41e2de8f 908e0a4be234c2fe 374e2c6e1fd217822dc08c0ced87b09ebc6afdec
//        let sha3Data = NSData(contentsOfFile: "/Users/vector/Downloads/thunder_3.3.3.4008.dmg")! as Data
//        self.items.append(WorkItem.init(tag: "m.sha3.224", block: {
//            let h = CBridage.sha3th224(sha3Data)
//            let result = h.toHexString()
//            self.results.append(result);
//        }));
//        self.items.append(WorkItem.init(tag: "m.sha3.256", block: {
//            let h = CBridage.sha3th256(sha3Data)
//            let result = h.toHexString()
//            self.results.append(result);
//        }));
//
        let data = NSData(contentsOfFile: "/Users/vector/Downloads/TIMSDK-master.zip")!
        let inputItem = data.subdata(with: NSMakeRange(0, 32 * 1024 * 1024))
        let buffer = NSMutableData(capacity: 4 * 32 * 1024 * 1024)!
        for _ in 0..<4 {
            buffer.append(inputItem)
        }
        let input = buffer as Data
        
//        self.items.append(WorkItem.init(tag: "m.sha3.224", block: {
//            let h = CBridage.sha3th224(input)
//            let result = h.toHexString()
//            self.results.append(result);
//        }));
//        self.items.append(WorkItem.init(tag: "m.sha3.256", block: {
//            let h = CBridage.sha3th256(input)
//            let result = h.toHexString()
//            self.results.append(result);
//        }));
//
        self.items.append(WorkItem.init(tag: "m.md5", block: {
            let h = CBridage.md5(input)
            let result = h.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.md5", block: {
            let h = NSMutableData(length: Int(CC_MD5_DIGEST_LENGTH))!

            CC_MD5(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));

        self.items.append(WorkItem.init(tag: "m.sha1", block: {
            let h = CBridage.sha160(input)
            let result = h.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.sha1", block: {
            let h = NSMutableData(length: Int(CC_SHA1_DIGEST_LENGTH))!
            CC_SHA1(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        
        self.items.append(WorkItem.init(tag: "m.sha2.224", block: {
            let sha2Oncea = CBridage.sha224(input)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.sha2.224", block: {
            let h = NSMutableData(length: Int(CC_SHA224_DIGEST_LENGTH))!
            CC_SHA224(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        

        self.items.append(WorkItem.init(tag: "m.sha2.256", block: {
            let sha2Oncea = CBridage.sha256(input)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.sha2.256", block: {
            let h = NSMutableData(length: Int(CC_SHA256_DIGEST_LENGTH))!
            CC_SHA256(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        
        
        self.items.append(WorkItem.init(tag: "m.sha2.384", block: {
            let sha2Oncea = CBridage.sha384(input)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }));
        
        self.items.append(WorkItem.init(tag: "sys.sha2.384", block: {
            let h = NSMutableData(length: Int(CC_SHA384_DIGEST_LENGTH))!
            CC_SHA384(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        
        
        self.items.append(WorkItem.init(tag: "m.sha2.512", block: {
            let sha2Oncea = CBridage.sha512(input)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.sha2.512", block: {
            let h = NSMutableData(length: Int(CC_SHA512_DIGEST_LENGTH))!
            CC_SHA512(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        

        
        for item in self.items {
            item.perform()
        }
        self.items.removeAll()
        var idx = 0
        while idx < self.results.count {
            let a = self.results[idx]
            let b = self.results[idx+1]
            if a == b {
                print(a)
            } else {
                print("!!!!! \(a) \(b)")
            }
            idx += 2
        }
        //        self.items.append(WorkItem.init(tag: "swift.md5", block: {
        //            let m = input.md5()
        //            let result = m.toHexString()
        //            self.results.append(result);
        //        }));
        //        self.items.append(WorkItem.init(tag: "swift.sha2.224", block: {
        //            let sha2Oncea = SHA2(variant: .sha224).calculate(for: input.bytes)
        //            let result = sha2Oncea.toHexString()
        //            self.results.append(result);
        //        }));
        //        self.items.append(WorkItem.init(tag: "swift.sha2.256", block: {
        //            let sha2Oncea = SHA2(variant: .sha256).calculate(for: input.bytes)
        //            let result = sha2Oncea.toHexString()
        //            self.results.append(result);
        //        }));
        //        self.items.append(WorkItem.init(tag: "swift.sha2.384", block: {
        //            let sha2Oncea = SHA2(variant: .sha384).calculate(for: input.bytes)
        //            let result = sha2Oncea.toHexString()
        //            self.results.append(result);
        //        }));
        //        self.items.append(WorkItem.init(tag: "swift.sha2.512", block: {
        //            let sha2Oncea = SHA2(variant: .sha512).calculate(for: input.bytes)
        //            let result = sha2Oncea.toHexString()
        //            self.results.append(result);
        //        }));
        // SHA2
        
        // SHA1
        //        let sha1Once = SHA1().calculate(for: input.bytes)
        
        //        var sha1Partial = SHA1()
        //        for batch in array {
        //            _ = try sha1Partial.update(withBytes: batch.bytes)
        //        }
        //        let sha1Result = try sha1Partial.finish()
        
        //        let b = CFAbsoluteTimeGetCurrent();
        //
        //
        //        // SHA2
        //        let sha2Once = SHA2(variant: .sha256).calculate(for: input.bytes)
        //
        ////        var sha2Partial = SHA2(variant: .sha256)
        ////        for batch in array {
        ////            _ = try sha2Partial.update(withBytes: batch.bytes)
        ////        }
        ////        let sha2Result = try sha2Partial.finish()
        //        let c = CFAbsoluteTimeGetCurrent();
        //
        //        // SHA3
        //        let sha3Once = SHA3(variant: .sha256).calculate(for: input.bytes)
        //
        ////        var sha3Partial = SHA3(variant: .sha256)
        ////        for batch in array {
        ////            _ = try sha3Partial.update(withBytes: batch.bytes)
        ////        }
        ////        let sha3Result = try sha3Partial.finish()
        //        let d = CFAbsoluteTimeGetCurrent();
        //
        //        // SHA3
        //        let sha3Once1 = SHA3(variant: .keccak256).calculate(for: input.bytes)
        //        let e = CFAbsoluteTimeGetCurrent();
        //
        //        print("\(e-d), \(d - c), \(c-b), \(b-a)")
        
    }
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        try! aa()
    }
    
    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    
    
}

