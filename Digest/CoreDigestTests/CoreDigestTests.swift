//
//  CoreDigestTests.swift
//  CoreDigestTests
//
//  Created by vector on 2019/7/31.
//  Copyright © 2019 angfung. All rights reserved.
//

import XCTest
@testable import CoreDigest


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

public final class SHA3 {
    var context: CDSHA3Context_s
    public enum Variant : UInt32 {
        case sha224 = 7,
        sha256,
        sha384,
        sha512,
        keccak224,
        keccak256, keccak384, keccak512
        
        
    }
    
    let v: SHA3.Variant
    public init(variant: SHA3.Variant) {
        v = variant
        context = CDSHA3Context_s()
        
        CDSHA3ContextInit(&context, CCDigestType_e(variant.rawValue))
    }
    
    public func calculate(for bytes: [UInt8]) -> Array<UInt8> {
        let dd = Data(bytes)
        let d: NSData = dd as NSData
        d.withContiguousStorageIfAvailable { (p: UnsafeBufferPointer<UInt8>) -> Void in
            CDSHA3Update(&context, p.baseAddress!, bytes.count)
        }
        CDSHA3Final(&context);
        
        let result = NSMutableData.init(length: 200)!
        CDSHA3ExportHashValue(&context, result.mutableBytes.assumingMemoryBound(to: UInt8.self))

        let le = CCDigestBlockSize( CCDigestType_e(v.rawValue))
        let r = result.subdata(with: NSMakeRange(0, le))
        return r.bytes
    }
}


class CoreDigestTests: XCTestCase {

    override func setUp() {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testExample() {
        
//        XCTAssertEqual(SHA3(variant: .keccak224).calculate(for: Array<UInt8>(hex: "")).toHexString(), "f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd")
//        XCTAssertEqual(SHA3(variant: .keccak256).calculate(for: Array<UInt8>(hex: "")).toHexString(), "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470")
//        XCTAssertEqual(SHA3(variant: .keccak384).calculate(for: Array<UInt8>(hex: "")).toHexString(), "2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff")
//        XCTAssertEqual(SHA3(variant: .keccak512).calculate(for: Array<UInt8>(hex: "")).toHexString(), "0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e")
        
        XCTAssertEqual(SHA3(variant: .sha224).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf")
        XCTAssertEqual(SHA3(variant: .sha256).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532")
        XCTAssertEqual(SHA3(variant: .sha384).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25")
        XCTAssertEqual(SHA3(variant: .sha512).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0")
        XCTAssertEqual(SHA3(variant: .keccak224).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "c30411768506ebe1c2871b1ee2e87d38df342317300a9b97a95ec6a8")
        XCTAssertEqual(SHA3(variant: .keccak256).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "4e03657aea45a94fc7d47ba826c8d667c0d1e6e33a64a036ec44f58fa12d6c45")
        XCTAssertEqual(SHA3(variant: .keccak384).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "f7df1165f033337be098e7d288ad6a2f74409d7a60b49c36642218de161b1f99f8c681e4afaf31a34db29fb763e3c28e")
        XCTAssertEqual(SHA3(variant: .keccak512).calculate(for: Array<UInt8>(hex: "616263")).toHexString(), "18587dc2ea106b9a1563e32b3312421ca164c7f1f07bc922a9c83d77cea3a1e5d0c69910739025372dc14ac9642629379540c17e2a65b19d77aa511a9d00bb96")
        
        XCTAssertEqual(SHA3(variant: .sha224).calculate(for: Array<UInt8>(hex: "")).toHexString(), "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7")
        XCTAssertEqual(SHA3(variant: .sha256).calculate(for: Array<UInt8>(hex: "")).toHexString(), "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a")
        XCTAssertEqual(SHA3(variant: .sha384).calculate(for: Array<UInt8>(hex: "")).toHexString(), "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004")
        XCTAssertEqual(SHA3(variant: .sha512).calculate(for: Array<UInt8>(hex: "")).toHexString(), "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26")
        XCTAssertEqual(SHA3(variant: .keccak224).calculate(for: Array<UInt8>(hex: "")).toHexString(), "f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd")
        XCTAssertEqual(SHA3(variant: .keccak256).calculate(for: Array<UInt8>(hex: "")).toHexString(), "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470")
        XCTAssertEqual(SHA3(variant: .keccak384).calculate(for: Array<UInt8>(hex: "")).toHexString(), "2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff")
        XCTAssertEqual(SHA3(variant: .keccak512).calculate(for: Array<UInt8>(hex: "")).toHexString(), "0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e")

        
        
        
        
        
        
    }

    func testPerformanceExample() {
        // This is an example of a performance test case.
        self.measure {
            // Put the code you want to measure the time of here.
        }
    }

}
