//
//  Manager.swift
//  FileSystem
//
//  Created by vector on 2019/12/1.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation

/*
 证书数据格式
 cer = {
     info = {
         version = Int32
         demain = ""
         isFinal = Bool
         user = ""
         publicKey = ""
         group = ""
         parent = Byte
         level = Int
         rw = ["xxx/xxx", "xxxx/xxxx"],
         r = ["xxx/xxx", "xxxx/xxxx"],
         endTime = 123234
     }
    parent = cer?
    identifier = "sha2"
    sign = ""
 }
 */

/*
文件系统结构
 fs = {
 box = [
    {
        home = {
        document = {}
        library = {}
        cache = {}
        tmp = {}
        }
        trash = {}
        log = {}
        cer = ""
    }
 ]
 share = {
 
 
 }
 storage = {
 
 
 }
 root_cer = ""
 
 }

*/

/*
 存储结构
 
 | -- 索引层 -- |
 | -- 数据层 -- |
 
 */



/*
 操作:
 add [file/dir]
 remove [file/dir]
 rename
 move a to b
 replace
 ls
 trash ls
 trash remove
 trash revert
 */

/*
 启动流程
 未完成的操作 进行完成
 
 */



public final class Manager {

}
public final class Session {

}
