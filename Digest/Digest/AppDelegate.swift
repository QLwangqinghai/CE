//
//  AppDelegate.swift
//  Digest
//
//  Created by vector on 2019/7/27.
//  Copyright © 2019 angfung. All rights reserved.
//

import Cocoa
import CommonCrypto
import CoreDigest

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



extension Array where Element == UInt8 {
    public init(hex: String) {
        self.init()
        var buffer: UInt8?
        var skip = hex.hasPrefix("0x") ? 2 : 0
        for char in hex.unicodeScalars.lazy {
            guard skip == 0 else {
                skip -= 1
                continue
            }
            guard char.value >= 48 && char.value <= 102 else {
                removeAll()
                return
            }
            let v: UInt8
            let c: UInt8 = UInt8(char.value)
            switch c {
            case let c where c <= 57:
                v = c - 48
            case let c where c >= 65 && c <= 70:
                v = c - 55
            case let c where c >= 97:
                v = c - 87
            default:
                removeAll()
                return
            }
            if let b = buffer {
                append(b << 4 | v)
                buffer = nil
            } else {
                buffer = v
            }
        }
        if let b = buffer {
            append(b)
        }
    }
    
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


extension Data {
    
    public init(hex: String) {
        self.init(Array<UInt8>(hex: hex))
    }
    public var bytes: Array<UInt8> {
        return Array(self)
    }
    
    public func toHexString() -> String {
        return bytes.toHexString()
    }
}



/*
 XCTAssertEqual(SHA3(variant: .sha224).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf")
 XCTAssertEqual(SHA3(variant: .sha256).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532")
 XCTAssertEqual(SHA3(variant: .sha384).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25")
 XCTAssertEqual(SHA3(variant: .sha512).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0")
 XCTAssertEqual(SHA3(variant: .keccak224).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "c30411768506ebe1c2871b1ee2e87d38df342317300a9b97a95ec6a8")
 XCTAssertEqual(SHA3(variant: .keccak256).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "4e03657aea45a94fc7d47ba826c8d667c0d1e6e33a64a036ec44f58fa12d6c45")
 XCTAssertEqual(SHA3(variant: .keccak384).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "f7df1165f033337be098e7d288ad6a2f74409d7a60b49c36642218de161b1f99f8c681e4afaf31a34db29fb763e3c28e")
 XCTAssertEqual(SHA3(variant: .keccak512).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "18587dc2ea106b9a1563e32b3312421ca164c7f1f07bc922a9c83d77cea3a1e5d0c69910739025372dc14ac9642629379540c17e2a65b19d77aa511a9d00bb96")
 
 
 */

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
        
        self.items.append(WorkItem(tag: "m.sha3.224(6)", block: {
            let h = CBridage.sha3th224(Data.init(hex: "616263"))
            let result = h.toHexString()
            print("\(224)  \(result) \(result == "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf")")
        }));
        
        
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
//        self.results.indices.reduce
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

