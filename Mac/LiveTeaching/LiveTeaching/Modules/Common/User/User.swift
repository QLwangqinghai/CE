//
//  User.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/28.
//  Copyright © 2020 vector. All rights reserved.
//

import Foundation

/// 性别
public enum Gender: UInt8 {
    /// 未知
    case unknown = 0
    
    /// 男
    case male = 1
    
    /// 女
    case female = 2
}

public class User {
    public let id: String
    public var nickname: String = ""
    public var gender: Gender = .unknown
    public var profilePhoto: String = ""
    public var region: String = ""
    public let account: String
    public var phone: String = ""
    public var updateTime: String = ""

    public init(id: String, account: String) {
        self.id = id
        self.account = account
    }    
}
