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

 21(60/3 + 4/4)级目录，每3字节1个目录
 3/3/3/3/......
 
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

public final class FileStorage {

}
