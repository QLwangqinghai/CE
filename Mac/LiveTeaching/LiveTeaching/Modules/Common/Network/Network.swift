//
//  Network.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit
import UIFoundation
import SwiftyBeaver
import Alamofire

public class Network: NSObject {
    public struct Service {
        public static let device: String = "com.angfung.device"
        
    }
    public struct Account {
        public static let identifier: String = "identifier"
    }

    public static let v: String = "id-seed"

    public static let accessGroup: String = "group.com.angfung"
    
    static func query() throws -> Data? {
        var query = [String: Any]()
        
        //通用密码
        query[String(kSecClass)] = String(kSecClassGenericPassword)
        query[String(kSecAttrAccessGroup)] = Network.accessGroup

        //禁止iCloud同步
        query[String(kSecAttrSynchronizable)] = kCFBooleanFalse
        query[String(kSecAttrAccessible)] = kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly
        query[String(kSecMatchLimit)] = kSecMatchLimitOne
        query[String(kSecReturnData)] = kCFBooleanTrue
        
        query[String(kSecAttrService)] = Service.device
        query[String(kSecAttrAccount)] = Account.identifier
        
        
        
        var result: AnyObject?
        let status = SecItemCopyMatching(query as CFDictionary, &result)
        switch status {
        case errSecSuccess:
            SwiftyBeaver.info("errSecSuccess")
            guard let object = result else {
                return nil
            }
            guard let data = object as? Data else {
                throw NSError.init(domain: "Network", code: 0, userInfo: ["message":"\(result)"])
            }
            return data
        case errSecItemNotFound:
            SwiftyBeaver.info("errSecItemNotFound")
            return nil
        default:
            SwiftyBeaver.info("\(status)")
            throw NSError.init(domain: "Network", code: 0, userInfo: ["message":"query: \(status)"])
        }
    }
    
    static func save(_ data: Data) throws {
        var query = [String: Any]()
        query[String(kSecClass)] = String(kSecClassGenericPassword)
        query[String(kSecAttrAccessGroup)] = Network.accessGroup

        //禁止iCloud同步
        query[String(kSecAttrSynchronizable)] = kCFBooleanFalse
        query[String(kSecAttrAccessible)] = kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly

        query[String(kSecUseAuthenticationUI)] = String(kSecUseAuthenticationUIFail)

        query[String(kSecAttrService)] = Service.device
        query[String(kSecAttrAccount)] = Account.identifier

        var status = SecItemCopyMatching(query as CFDictionary, nil)
        switch status {
        case errSecSuccess:
            status = SecItemDelete(query as CFDictionary)
            if status != errSecSuccess {

            }
        default:
            break
        }
        query[String(kSecValueData)] = data
        status = SecItemAdd(query as CFDictionary, nil)
        
        if status != errSecSuccess {
            SwiftyBeaver.info("\(status)")
            throw NSError.init(domain: "Network", code: 0, userInfo: ["message":"\(status)"])
        }
        SwiftyBeaver.info("success")
    }


    static func test() {
//        RequestInterceptor

//        var d: DataRequest?
//        d?.responseJSON(completionHandler: { (<#AFDataResponse<Any>#>) in
//            <#code#>
//        })
//        
//        AF.request("www.baidu.com", method: HTTPMethod.get, parameters: Encodable?, encoder: ParameterEncoder, headers: HTTPHeaders.init(), interceptor: RequestInterceptor?).response
//        
//        
//        do {
//            let data = try Network.query()
//            
//            SwiftyBeaver.info("query data: \(data)")
//            
//            if data == nil {
//                let d = UUID().uuidString.data(using: String.Encoding.utf8)!
//                try Network.save(d)
//            }
//            
//            let data2 = try Network.query()
//            
//            SwiftyBeaver.info("2 query data: \(data2)")
//
//        } catch let error {
//            print(error)
//        }
    }
    
    
}


extension Network {
    public struct User {
//        public static let
        
        
    }
}
