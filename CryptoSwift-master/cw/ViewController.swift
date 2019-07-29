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
import CryptoSwift

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        
        let sha3Data = NSData(contentsOfFile: "/Users/vector/Downloads/thunder_3.3.3.4008.dmg")! as Data
//        let sha3Once = SHA3(variant: SHA3.Variant.sha224).calculate(for: sha3Data.bytes)
//        let result = sha3Once.toHexString()
//        print(result)

        
        let sha3Once = SHA3(variant: SHA3.Variant.sha256).calculate(for: sha3Data.bytes)
        let result = sha3Once.toHexString()
        print(result)
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

