//
//  UserGroup.swift
//  FileManager
//
//  Created by vector on 2019/7/30.
//  Copyright © 2019 angfung. All rights reserved.
//

import Foundation


//组是访问文件系统的最小单位， 用户可以归属于不同群组

//组的分类
/**
 超级系统管理员组 组成员只能有一个， SS
 系统管理员组 S.
 私人用户组 组成员只能有一个 P.
 普通用户组 成员人数>=1
 Any组
 
 */
/*
 权限：
 增加
 删除
 读取
 */


/*
 访问层
 应用接口层
 数据存储层
 */

/** user id  uint64_t*/


/** group type  uint8_t [1, 255]*/
/** group id  uint64_t*/
