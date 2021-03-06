////  CryptoSwift
//
//  Copyright (C) 2014-__YEAR__ Marcin Krzyżanowski <marcin@krzyzanowskim.com>
//  This software is provided 'as-is', without any express or implied warranty.
//
//  In no event will the authors be held liable for any damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
//  - The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation is required.
//  - Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//  - This notice may not be removed or altered from any source or binary distribution.
//

import Cocoa
//import CryptoSwift
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
        let sha3Data = NSData(contentsOfFile: "/Users/vector/Downloads/thunder_3.3.3.4008.dmg")
        let wi = WorkItem(tag: "m.sha3.256", block: {
            let sha2Oncea =

            
            
            let sha2Oncea = CBridage.sha256(sha3Data)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }
        
        
        var data = NSData(contentsOfFile: "/Users/vector/Downloads/TIMSDK-master.zip")!
        
        var array: [Data] = []
        for idx in 0..<32 {
            let a = data.subdata(with: NSMakeRange(idx * 1024 * 1024, 1024 * 1024))
            array.append(a)
        }
        let ax: CC_MD5_CTX = CC_MD5_CTX()
        
        let input = data.subdata(with: NSMakeRange(0, 32 * 1024 * 1024))
        

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
        

        
        
        self.items.append(WorkItem.init(tag: "m.sha2.224", block: {
            let sha2Oncea = CBridage.sha224(input)
            let result = sha2Oncea.toHexString()
            self.results.append(result);
        }));
        self.items.append(WorkItem.init(tag: "sys.sha.224", block: {
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
        self.items.append(WorkItem.init(tag: "sys.sha.256", block: {
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
        
        self.items.append(WorkItem.init(tag: "sys.sha.384", block: {
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
        self.items.append(WorkItem.init(tag: "sys.sha.512", block: {
            let h = NSMutableData(length: Int(CC_SHA512_DIGEST_LENGTH))!
            CC_SHA512(input.bytes, UInt32(input.count), h.mutableBytes.assumingMemoryBound(to: UInt8.self))
            let result = (h as Data).toHexString()
            self.results.append(result);
        }));
        
        for item in self.items {
            item.perform()
        }
        print("\(self.results)")
        
        let a = CFAbsoluteTimeGetCurrent();

        
        
        
        
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

