//
//  FileStorage.swift
//  Alamofire
//
//  Created by vector on 2020/2/27.
//

import Foundation

/*
 15年款macBookpro
 7.84Gb
 
 openssl dgst 效率测试
 
 //1轮
 MD5 used: 17.80396604537964
 SHA256 used: 37.035207986831665
 SHA512 used: 26.20375108718872
 
 //2轮
 MD5 used: 17.358261108398438
 SHA256 used: 36.07696604728699
 SHA512 used: 25.850743055343628

  */

/*
 
 64字节

 22(4/2 + 60/3) - 1级目录
 1/3/3/3/3/......
 
 */

/*
 分段dgst
 16MB/段
 */

/*
 sha512/filelength
 
 */

/*
存储结构

| -- 索引层 -- |
| -- 数据层 -- |

*/

/*
文件系统结构
fs.storage = {


}
*/

public struct FileStorageConfig {
//    public let dgst: UInt8
//    public let homeDirectoryPath: String
//    public let fileManager: FileManager
//
//    public init(dgst: UInt8, homeDirectoryPath: String) {
//        self.homeDirectoryPath = homeDirectoryPath
//        let fileManager: FileManager = FileManager()
//        let
//
//
//
//
//
//
//        guard fileManager.changeCurrentDirectoryPath(homeDirectoryPath) else {
//            fatalError("directoryPath:\(directoryPath) error")
//        }
//        self.dgst = dgst
//        self.fileManager = fileManager
//    }
}



public final class FileStorage {
//    public let dgst: UInt8
//    public let homeDirectoryPath: String
//    public let fileManager: FileManager
//    
//    public init(dgst: UInt8, homeDirectoryPath: String) {
//        self.homeDirectoryPath = homeDirectoryPath
//        let fileManager: FileManager = FileManager()
//        
//        guard fileManager.changeCurrentDirectoryPath(homeDirectoryPath) else {
//            fatalError("directoryPath:\(directoryPath) error")
//        }
//        self.dgst = dgst
//        self.fileManager = fileManager
//    }
}
